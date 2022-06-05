#pragma once

#include <vector>
#include "./glad/glad.h"
#include "./GLFW/glfw3.h"
#include <iostream>

static float Vertces[] = {
    // pos       //texcoords
    -1.0f, 1.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,

    -1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 1.0f, 1.0f};

class FrameBuffer
{
public:
    int ScreenWidth;
    int ScreenHeight;
    int Samples; // the value of Samples should not be Changed after Init.
    unsigned int VBO;
    unsigned int VAO;
    std::vector<unsigned int> texture_attachments;
    unsigned int renderbuffer;
    unsigned int ID;
    int texturelayers;

    // G_Buffer Settings
    bool noInit;

    FrameBuffer(int width, int height, int samplesamount = 1, int layers = 1, bool no_init = false)
    {
        ScreenWidth = width;
        ScreenHeight = height;
        Samples = samplesamount;
        texturelayers = layers;
        noInit = no_init;

        vao_vbo_config();

        // Framebuffer Instance will not be Filled when noInit is Activated <used for G_Buffer>
        if(!noInit) {
            build();
            Check();
        }

#ifdef _FRAMEBUFFER_DEBUG
        if (!no_init) {
            std::cout << std::endl;
            std::cout << "MANUAL_DEBUG::FRAMEBUFFER" << std::endl;
            std::cout << "MANUAL_DEBUG::FRAMEBUFFER::DATA" << std::endl;
            std::cout << "ID: " << ID << std::endl;
            std::cout << "Samples: " << Samples << std::endl;
            std::cout << "Resolution: " << ScreenWidth << " * " << ScreenHeight << std::endl;
            std::cout << "TextureAttachments: " << texturelayers << std::endl;
            std::cout << "\tTextures: " << std::endl;
            for (int i = 0; i < texture_attachments.size(); ++i)
                std::cout << "\tSlot: " << i << " || " << texture_attachments.at(i) << std::endl;
            if(Samples > 1)
            {
                std::cout << "Extra Textures for MultiSampling:" << std::endl;
                for (int i = 0; i < tmp_texture_attachments.size(); ++i)
                    std::cout << "\tSlot: " << i << " || " << tmp_texture_attachments.at(i) << std::endl;
            }
        }
#endif
    }

    void Delete()
    {
        glDeleteFramebuffers(1, &ID);
        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);

        if (Samples > 1)
            glDeleteFramebuffers(1, &tmpfbo);
    }

    std::vector<unsigned int> ServeTextures()
    {
        if(Samples > 1)
        {
            glBlitNamedFramebuffer(ID, tmpfbo, 0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            return tmp_texture_attachments;
        }
        else
            return texture_attachments;
    }

    void MRTRenderConfig()
    {
        std::vector<unsigned int> attachments;
        for (unsigned int i = 0; i < texturelayers; ++i) {
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
        }

        glNamedFramebufferDrawBuffers(ID, texturelayers, attachments.data());
    }

    void Draw(unsigned int texture)
    {
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
    }

    void Check()
    {
        // Check the Main Framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER::MAIN:: FrameBuffer is NOT Compelete." << std::endl;

        // Check the tmp FrameBuffer
        if (Samples > 1)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, tmpfbo);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER::MultiSampling:: FrameBuffer is NOT Compelete." << std::endl;
        }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

private:
    // Used for MultiSampling and Post Effect
    unsigned int tmpfbo;
    std::vector<unsigned int> tmp_texture_attachments;
    unsigned int tmp_render_buffer;

    void build()
    {
        glGenFramebuffers(1, &ID);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        // Texture_Attachment Settings
        glGenRenderbuffers(1, &renderbuffer);

        if (Samples == 1)
        {
            // Texture Attachment
            for (int i = 0; i < texturelayers; ++i) {
                //MRT
                unsigned int texture_attachment;
                glGenTextures(1, &texture_attachment);

                glBindTexture(GL_TEXTURE_2D, texture_attachment);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);    // GL_RGB16 for HDR Usage
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glBindTexture(GL_TEXTURE_2D, 0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture_attachment, 0);

                texture_attachments.push_back(texture_attachment);
            }

            // RenderBuffer Attachment
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Binding
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
        }
        else
        {
            // TextureAttachment
            for (int i = 0; i < texturelayers; ++i) {
                // MRT
                unsigned int texture_attachment;
                glGenTextures(1, &texture_attachment);

                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_attachment);
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, GL_RGB16F, ScreenWidth, ScreenHeight, GL_TRUE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, texture_attachment, 0);

                texture_attachments.push_back(texture_attachment);
            }

            // RenderBuffer Attachment
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Binding
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            // FrameBuffer Used for Sampling and Post Effect
            glGenFramebuffers(1, &tmpfbo);
            glBindFramebuffer(GL_FRAMEBUFFER, tmpfbo);

            glGenRenderbuffers(1, &tmp_render_buffer);

            for (int i = 0; i < texturelayers; ++i) {
                // MRT
                unsigned int tmp_texture_attachment;
                glGenTextures(1, &tmp_texture_attachment);

                glBindTexture(GL_TEXTURE_2D, tmp_texture_attachment);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glBindTexture(GL_TEXTURE_2D, 0);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tmp_texture_attachment, 0);

                tmp_texture_attachments.push_back(tmp_texture_attachment);
            }

            glBindRenderbuffer(GL_RENDERBUFFER, tmp_render_buffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, tmp_render_buffer);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    void vao_vbo_config()
    {
                // Load VAO and VBO for Screen
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertces), &Vertces, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
};
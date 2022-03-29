#pragma once

#include "./glad/glad.h"
#include "./GLFW/glfw3.h"
#include <iostream>

float Vertces[] = {
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
    int Samples; // Do not change the value of Samples after Init.
    unsigned int VBO;
    unsigned int VAO;
    unsigned int texture_attachment;
    unsigned int renderbuffer;
    unsigned int ID;

    FrameBuffer(int width, int height, int samplesamount = 1)
    {
        ScreenWidth = width;
        ScreenHeight = height;
        Samples = samplesamount;

        build();
    };

    void Delete()
    {
        glDeleteFramebuffers(1, &ID);
        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);

        if (Samples > 1)
            glDeleteFramebuffers(1, &tmpfbo);
    };

    unsigned int MultiSampledTexture2D()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, tmpfbo);
        glBlitFramebuffer(0, 0, ScreenWidth, ScreenHeight, 0, 0, ScreenWidth, ScreenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return tmp_texture_attachment;
    };

private:
    // Used for MultiSampling and Post Effect
    unsigned int tmpfbo;
    unsigned int tmp_texture_attachment;
    unsigned int tmp_render_buffer;

    void build()
    {
        glGenFramebuffers(1, &ID);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        // Texture_Attachment Settings
        glGenTextures(1, &texture_attachment);
        glGenRenderbuffers(1, &renderbuffer);

        if (Samples == 1)
        {
            // Texture Attachment
            glBindTexture(GL_TEXTURE_2D, texture_attachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            // RenderBuffer Attachment
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Bindings
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_attachment, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
        }
        else
        {
            // TextureAttachment
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture_attachment);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, Samples, GL_RGB, ScreenWidth, ScreenHeight, GL_TRUE);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

            // RenderBuffer Attachment
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, Samples, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            // Bindings
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture_attachment, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);


            // FrameBuffer Used for Sampling and Post Effect
            glGenFramebuffers(1, &tmpfbo);
            glBindFramebuffer(GL_FRAMEBUFFER, tmpfbo);

            glGenBuffers(1, &tmp_texture_attachment);
            glGenRenderbuffers(1, &tmp_render_buffer);

            glBindTexture(GL_TEXTURE_2D, tmp_texture_attachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindRenderbuffer(GL_RENDERBUFFER, tmp_render_buffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tmp_texture_attachment, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, tmp_render_buffer);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Check the Main Framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, ID);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER::MAIN:: FrameBuffer is NOT Compelete." << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Check the tmp FrameBuffer
        if (Samples > 1)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, tmpfbo);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER::MultiSampling:: FrameBuffer is NOT Compelete." << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

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
        // fin
    }
};

// Need further debug
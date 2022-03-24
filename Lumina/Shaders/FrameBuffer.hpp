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
    unsigned int VBO;
    unsigned int VAO;
    unsigned int texture_attachment;
    unsigned int renderbuffer;
    unsigned int ID;

    FrameBuffer(int width, int height)
    {
        ScreenWidth = width;
        ScreenHeight = height;

        build();
    };

    void Delete()
    {
        glDeleteFramebuffers(1, &ID);
        glDeleteBuffers(1, &VAO);
        glDeleteBuffers(1, &VBO);
    };

private:
    void build()
    {
        glGenFramebuffers(1, &ID);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        // Texture_Attachment Settings
        glGenTextures(1, &texture_attachment);
        glBindTexture(GL_TEXTURE_2D, texture_attachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Attach it to FrameBuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_attachment, 0);

        // RenderBuffers are usually Write_ONLY, so it mostly use for Storeing Depth and Stencil. we need Depth and Stencil for Depth_test and Stencil_test but hardly sampling them
        unsigned int renderbuffer;
        glGenRenderbuffers(1, &renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // Attach it to FrameBuffer
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

        // Check the Bound Framebuffer
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: FrameBuffer is NOT Compelete." << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
#pragma once

#include "../glad/glad.h"
#include "../GLFW/glfw3.h"
#include "../Shader.hpp"
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

        FrameBuffer(int width, int height) {
            glGenFramebuffers(1, &ID);
            glBindFramebuffer(GL_FRAMEBUFFER, ID);

            bulidTexture_Attachment();
            bulidRender_Buffer();

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_attachment, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

            if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cout << "ERROR::FRAMEBUFFER:: FrameBuffer is NOT Compelete." << std::endl;

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            bulidVAO();
        };

    private:
        void bulidTexture_Attachment() {
            glGenTextures(1, &texture_attachment);
            glBindTexture(GL_TEXTURE_2D, texture_attachment);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ScreenWidth, ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        };

        void bulidRender_Buffer(){
            glGenRenderbuffers(1, &renderbuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, ScreenWidth, ScreenHeight);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        };

        void bulidVBO() {
            glGenBuffers(1,&VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertces), &Vertces, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        };

        void bulidVAO(){
            glGenBuffers(1, &VAO);
            glBindVertexArray(VAO);

            bulidVBO;

            glBindBuffer(GL_ARRAY_BUFFER,VBO);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        };
};
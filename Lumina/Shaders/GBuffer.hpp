// Used to Simplify Render Loop Code in Main Segment
#pragma once

#include "FrameBuffer.hpp"
#include "../Shader.hpp"

const int texture_layers = 3;

class GBuffer
{
public:
    FrameBuffer fb;
    int SCRWidth;
    int SCRHeight;
    unsigned int gPosition;
    unsigned int gNormal;
    unsigned int gAlbedoSpec;

    /* 
    texture_attachment layout:
    0||gPostion   ||RBG32F
    1||gNormal    ||RGB32F
    2||gAlbedoSpec||RBA
    */

    GBuffer(int width, int height) : fb(width, height, 1, texture_layers, true)
    {
        SCRWidth = width;
        SCRHeight = height;

        glGenFramebuffers(1, &fb.ID);
        glBindFramebuffer(GL_FRAMEBUFFER, fb.ID);

        // gPosition
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCRWidth, SCRHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D,0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        fb.texture_attachments.push_back(gPosition);

        // gNormal
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCRWidth, SCRHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 1, GL_TEXTURE_2D, gNormal, 0);
        fb.texture_attachments.push_back(gNormal);

        // gAlbedoSpec
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCRWidth, SCRHeight, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D,0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + 2, GL_TEXTURE_2D, gAlbedoSpec, 0);
        fb.texture_attachments.push_back(gAlbedoSpec);

        //RBO Config
        glGenRenderbuffers(1, &fb.renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, fb.renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCRWidth, SCRHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb.renderbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Status Check
        fb.Check();

#ifdef _FRAMEBUFFER_DEBUG
        std::cout << std::endl;
        std::cout << "MANUAL_DEBUG::G_BUFFER" << std::endl;
        std::cout << "MANUAL_DEBUG::G_BUFFER::DATA" << std::endl;
        std::cout << "ID: " << fb.ID << std::endl;
        std::cout << "Resolution: " << SCRWidth << " * " << SCRHeight << std::endl;
        std::cout << "TextureAttachments: " << fb.texturelayers << std::endl;
        std::cout << "\tTextures: " << std::endl;
        for (int i = 0; i < fb.texture_attachments.size(); ++i)
            std::cout << "\tSlot: " << i << " || " << fb.texture_attachments.at(i) << std::endl;
#endif
    }

    // GL_TEXTURE1 ~ 6 Reserved for Deferred Rendering
    void Deferred_Rendering_Config(Shader* _lighting_pass_shader)
    {
        _lighting_pass_shader->setInt("gbuffertex.gPosition", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gPosition);

        _lighting_pass_shader->setInt("gbuffertex.gNormal", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gNormal);

        _lighting_pass_shader->setInt("gbuffertex.gAlbedoSpec", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};
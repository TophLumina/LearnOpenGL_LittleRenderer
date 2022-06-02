// Used to Simplify Render Loop Code in Main Segment
#pragma once

#include "FrameBuffer.hpp"

const int texture_layers = 3;

class GBuffer
{
public:
    FrameBuffer GB;
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


    GBuffer(int width, int height) : GB(width, height, 1, texture_layers, true)
    {
        SCRWidth = width;
        SCRHeight = height;

        glBindFramebuffer(GL_FRAMEBUFFER, GB.ID);

        // gPosition
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCRWidth, SCRHeight, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D,0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        GB.texture_attachments.push_back(gPosition);

        // gNormal
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, SCRWidth, SCRHeight, 0, GL_RGB, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        GB.texture_attachments.push_back(gNormal);

        // gAlbedoSpec
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCRWidth, SCRHeight, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D,0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
        GB.texture_attachments.push_back(gAlbedoSpec);

        //RBO Config
        glGenRenderbuffers(1, &GB.renderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, GB.renderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCRWidth, SCRHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GB.renderbuffer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Status Check
        GB.Check();
    }
};
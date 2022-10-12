#pragma once

#include <cmath>
#include <random>

#include "FrameBuffer.hpp"
#include "GBuffer.hpp"
#include "../Shader.hpp"

class SSAOtools
{
public:
    FrameBuffer SSAOfb;
    GBuffer* gBuffer;
    Shader* SSAOPassShader;
    
    // Blur
    FrameBuffer SSAOBlurfb;
    unsigned int BlurTexture;
    unsigned int SSAOfbTexture;
    Shader Blurshader;

    SSAOtools(int width, int height, GBuffer* gbuffer, Shader* shader, int _kernal_size = 64, int _noise_size = 4) :
    SSAOfb(width, height, 1, 1, true),
    SSAOBlurfb(width, height, 1, 1, true),
    Blurshader("./Shaders/HDR.vert","./Shaders/SSAOBlur.frag")
    {
        SRCWidth = width;
        SRCHeight = height;
        gBuffer = gbuffer;
        SSAOPassShader = shader;
        SSAOkernalsize = _kernal_size;
        SSAOnoisesize = _noise_size;

        buildSSAOkernal_SSAOnoise();
        buildSSAOframebuffers();
    }

    // Caution: This Func will NOT Set uniform_block of the Shader
    void ShaderConfig()
    {
        SSAOPassShader->setInt("SRC_Width", SRCWidth);
        SSAOPassShader->setInt("SRC_Height", SRCHeight);
        for (int i = 0; i < SSAOkernalsize; ++i)
            SSAOPassShader->setVec3("samplers[" + std::to_string(i) + "]", SSAOkernal[i]);
    }

    void LightingPass_Shader_Config(Shader* _lighting_pass_shader, bool _apply_bulr)
    {
        _lighting_pass_shader->setInt("ssao_compoent.SSAOTexture", 6);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, _apply_bulr ? BlurTexture : SSAOfbTexture);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Draw()
    {
        SSAOPassShader->setInt("SSAONoise", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, SSAONoiseTexture);

        SSAOPassShader->setInt("gPosition_View", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer->gPosition_View);

        SSAOPassShader->setInt("gNormal_View", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, gBuffer->gNormal_View);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, SSAOfb.ID);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SSAOfb.Draw();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Blur
        Blurshader.Use();

        glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurfb.ID);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SSAOBlurfb.Draw(SSAOfbTexture);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

private:
    int SRCWidth;
    int SRCHeight;
    int SSAOkernalsize;
    int SSAOnoisesize;

    std::vector<glm::vec3> SSAOkernal;
    std::vector<glm::vec3> SSAOnoise;
    unsigned int SSAONoiseTexture;

    void buildSSAOnoisetexture()
    {
        glGenTextures(1, &SSAONoiseTexture);
        glBindTexture(GL_TEXTURE_2D, SSAONoiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SSAOnoisesize, SSAOnoisesize, 0, GL_RGB, GL_FLOAT, &SSAOnoise[0]);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // SSAOkernal & SSAOnoise builder
    void buildSSAOkernal_SSAOnoise()
    {
        std::uniform_real_distribution<float> distributor(0.0f, 1.0f);
        std::default_random_engine generator;

        for (int i = 0; i < SSAOkernalsize; ++i) {
            glm::vec3 sampler(
                (distributor(generator) * 2.0f - 1.0f),
                (distributor(generator) * 2.0f - 1.0f),
                distributor(generator));
            sampler = glm::normalize(sampler);
            sampler = sampler * distributor(generator);
            float scale = float(i) / SSAOkernalsize;
            scale = std::lerp(0.1f, 1.0f, scale * scale);
            sampler *= scale;
            SSAOkernal.push_back(sampler);
        }

        for (int i = 0; i < SSAOnoisesize * SSAOkernalsize; ++i) {
            glm::vec3 noise(
                distributor(generator) * 2.0f - 1.0f,
                distributor(generator) * 2.0f - 1.0f,
                0.0f);
            SSAOnoise.push_back(noise);
        }

        buildSSAOnoisetexture();
    }

    // SSAOframebuffer builder
    void buildSSAOframebuffers()
    {
        glGenFramebuffers(1, &SSAOfb.ID);
        glBindFramebuffer(GL_FRAMEBUFFER, SSAOfb.ID);

        glGenTextures(1, &SSAOfbTexture);
        glBindTexture(GL_TEXTURE_2D, SSAOfbTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SRCWidth, SRCHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOfbTexture, 0);
        SSAOfb.texture_attachments.push_back(SSAOfbTexture);

        glBindTexture(GL_TEXTURE_2D,0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        SSAOfb.Check();

        glGenFramebuffers(1, &SSAOBlurfb.ID);
        glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurfb.ID);

        glGenTextures(1, &BlurTexture);
        glBindTexture(GL_TEXTURE_2D, BlurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SRCWidth, SRCHeight, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, BlurTexture, 0);
        SSAOBlurfb.texture_attachments.push_back(BlurTexture);

        glBindTexture(GL_TEXTURE_2D,0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        SSAOBlurfb.Check();
    }
};
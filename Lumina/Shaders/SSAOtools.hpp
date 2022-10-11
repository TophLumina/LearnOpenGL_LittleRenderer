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
    int SRCWidth;
    int SRCHeight;
    unsigned int SSAONoiseTexture;
    unsigned int SSAOfbTexture;
    std::vector<glm::vec3> SSAOkernal;
    GBuffer* gBuffer;   // remember check when apply SSAO
    Shader* SSAOPassShader;

    int SSAOkernalsize;
    int SSAOnoisesize;

    SSAOtools(int width, int height, int _kernal_size = 64, int _noise_size = 4) : SSAOfb(width, height, 1, 1, true)
    {
        SRCWidth = width;
        SRCHeight = height;
        SSAOkernalsize = _kernal_size;
        SSAOnoisesize = _noise_size;

        buildSSAOkernal();

        SSAOPassShader->Use();
        for (int i = 0; i < SSAOkernalsize; ++i)
            SSAOPassShader->setVec3("samplers[" + std::to_string(i) + "]", SSAOkernal[i]);

        glGenFramebuffers(1, &SSAOfb.ID);
        glBindFramebuffer(GL_FRAMEBUFFER, SSAOfb.ID);

    }

private:
    std::vector<glm::vec3> SSAOkernal;

    // SSAOkernal builder
    void buildSSAOkernal()
    {
        std::uniform_real_distribution<float> distributor(0.0f, 1.0f);
        std::default_random_engine generator;

        for (int i = 0; i < SSAOkernalsize; ++i) {
            glm::vec3 sampler(
                (distributor(generator) * 2.0f - 1.0f),
                (distributor(generator) * 2.0f - 1.0f),
                distributor(generator));
            sampler = glm::normalize(sampler);
            sampler *= distributor(generator);
            float scale = float(i) / SSAOkernalsize;
            scale = std::lerp(0.1f, 1.0f, scale * scale);
            sampler *= scale;
            SSAOkernal.push_back(sampler);
        }
    }

    
};
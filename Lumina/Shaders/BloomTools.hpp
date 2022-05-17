#pragma once

#include <vector>

#include"../Shader.hpp"
#include "./FrameBuffer.hpp"

class BloomTool
{
public:
    BloomTool(FrameBuffer *_origin_fb, Shader *_blur_shader, Shader *_mix_shader)
    {
        blur_shader = _blur_shader;
        mix_shader = _mix_shader;
        origin_color = _origin_fb->ServeTextures().at(0);
        origin_bright = _origin_fb->ServeTextures().at(1);

        blur_fbs.push_back(FrameBuffer(_origin_fb->ScreenWidth, _origin_fb->ScreenHeight, _origin_fb->Samples));
        blur_fbs.push_back(FrameBuffer(_origin_fb->ScreenWidth, _origin_fb->ScreenHeight, _origin_fb->Samples));
    }

    void ApplyBloom(int loop)
    {
        Bloom(loop);
        Mix();
    }

    unsigned int tex_finished()
    {
        return blur_fbs.at(0).ServeTextures().at(0);
    }

private:
    std::vector<FrameBuffer> blur_fbs;
    unsigned int origin_color;
    unsigned int origin_bright;
    Shader *blur_shader;
    Shader *mix_shader;

    void Bloom(int loop)
    {
        bool enter = true;
        blur_shader->Use();

        for (int i = 0; i < loop; ++i)
        {
            // Horizontal
            glBindFramebuffer(GL_FRAMEBUFFER, blur_fbs.at(0).ID);
            glDisable(GL_DEPTH_TEST);   // When Using FrameBuffers Remind Yourself to Disable Depth_Test otherwise the Screen will be Invisible
            glClear(GL_COLOR_BUFFER_BIT);
            blur_shader->setBool("horizontal", true);
            blur_fbs.at(0).Draw(enter ? origin_bright : blur_fbs.at(1).ServeTextures().at(0));

            enter = false;

            // Vertical
            glBindFramebuffer(GL_FRAMEBUFFER, blur_fbs.at(1).ID);
            glClear(GL_COLOR_BUFFER_BIT);
            blur_shader->setBool("horizontal", false);
            blur_fbs.at(1).Draw(blur_fbs.at(0).ServeTextures().at(0));
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Mix()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, blur_fbs.at(0).ID);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        mix_shader->Use();
        glActiveTexture(GL_TEXTURE1);
        mix_shader->setInt("color", 1);
        glBindTexture(GL_TEXTURE_2D, origin_color);

        glActiveTexture(GL_TEXTURE2);
        mix_shader->setInt("bloomblur", 2);
        glBindTexture(GL_TEXTURE_2D, blur_fbs.at(1).ServeTextures().at(0));

        glActiveTexture(GL_TEXTURE0);
        blur_fbs.at(0).Draw(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};
#pragma once

#include "./glm/glm.hpp"
#include "./Attachments.hpp"

class SpotLight {

public:
    glm::vec3 position;
    glm::vec3 direction;
    LightAttrib attrib;
    Attenuation attenuation;
    float cutoff;
    float outtercutoff;

    SpotLight(LightAttrib attrib, glm::vec3 pos, glm::vec3 dir, Attenuation attenuation, float a, float b) {
        this->attrib = attrib;
        this->position = pos;
        this->direction = dir;
        this->attenuation = attenuation;
        this->cutoff = a;
        this->outtercutoff = b;
    };
};
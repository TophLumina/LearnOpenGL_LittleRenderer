#pragma once

#include "./glm/glm.hpp"
#include "./Attachments.hpp"

class PointLight {

public:
    glm::vec3 position;
    LightAttrib attrib;
    Attenuation attenuation;

    PointLight(LightAttrib attrib, glm::vec3 pos, Attenuation attenuation) {
        this->attrib = attrib;
        this->position = pos;
        this->attenuation = attenuation;
    };
};
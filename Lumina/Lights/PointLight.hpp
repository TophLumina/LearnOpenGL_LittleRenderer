#pragma once

#include "./glm/glm.hpp"
#include "./Attachments.hpp"

class PointLight {

public:
    glm::vec3 position;
    LightAttrib attrib;
    Attenuation attenuation;
    glm::mat4 lightMatrix;
    unsigned int depthmap;

    PointLight(LightAttrib attrib, glm::vec3 pos, Attenuation attenuation, glm::mat4 matrix ,unsigned int depth) {
        this->attrib = attrib;
        this->position = pos;
        this->attenuation = attenuation;
        this->lightMatrix = matrix;
        this->depthmap = depth;
    };
};
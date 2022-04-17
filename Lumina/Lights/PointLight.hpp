#pragma once

#include "./glm/glm.hpp"
#include "./Attachments.hpp"

class PointLight {

public:
    glm::vec3 position;
    LightAttrib attrib;
    Attenuation attenuation;
    unsigned int depthmap;
    float far;

    PointLight(LightAttrib attrib, glm::vec3 pos, Attenuation attenuation, unsigned int depth, float far_plane) {
        this->attrib = attrib;
        this->position = pos;
        this->attenuation = attenuation;
        this->depthmap = depth;
        this->far = far_plane;
    };
};
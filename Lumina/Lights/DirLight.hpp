#pragma once

#include "./glm/glm.hpp"
#include "./Attachments.hpp"

class DirLight {

public:
    glm::vec3 direction;
    LightAttrib attrib;
    glm::mat4 lightMatrix;
    unsigned int depthmap;

    DirLight(LightAttrib attrib, glm::vec3 dir, glm::mat4 matrix, unsigned int depth) {
        this->attrib = attrib;
        this->direction = dir;
        this->lightMatrix = matrix;
        this->depthmap = depth;
    };
};
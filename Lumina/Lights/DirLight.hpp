#pragma once

#include "./glm/glm.hpp"
#include "./Attachments.hpp"

class DirLight {

public:
    glm::vec3 direction;
    LightAttrib attrib;

    DirLight(LightAttrib attrib, glm::vec3 dir) {
        this->attrib = attrib;
        this->direction = dir;
    };
};
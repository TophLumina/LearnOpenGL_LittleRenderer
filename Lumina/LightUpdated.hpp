#pragma once

#include <string>
#include <iostream>

#include "Shader.hpp"
#include "glm/glm.hpp"

class Light {
    const std::string error_msg = "ERROR::Shader:: invalid Shader address.";

public:
    unsigned int num_Dirlight;
    unsigned int num_Pointlight;
    unsigned int num_Spotlight;
    Shader *shader;

    Light(Shader *aShader) {
        this->num_Dirlight = 0;
        this->num_Pointlight = 0;
        this->num_Spotlight = 0;
        this->shader = aShader;
    }

    void error() const {
        if (!this->shader)
            std::cout << error_msg << std::endl;
    }

    void update() const {
        this->shader->setInt("num_dirlight", num_Dirlight);
        this->shader->setInt("num_pointlight", num_Pointlight);
        this->shader->setInt("num_spotlight", num_Spotlight);
    }

    void updateDirlight(unsigned int slot, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
        error();

        this->shader->setVec3("dirlights[" + std::to_string(slot) + "].direction", direction);
        this->shader->setVec3("dirlights[" + std::to_string(slot) + "].attrib.ambient", ambient);
        this->shader->setVec3("dirlights[" + std::to_string(slot) + "].attrib.diffuse", diffuse);
        this->shader->setVec3("dirlights[" + std::to_string(slot) + "].attrib.specular", specular);
        update();
    }

    void updatePointlight(unsigned int slot, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic) {
        error();

        this->shader->setVec3("pointlights[" + std::to_string(slot) + "].position", position);
        this->shader->setVec3("pointlights[" + std::to_string(slot) + "].attrib.ambient", ambient);
        this->shader->setVec3("pointlights[" + std::to_string(slot) + "].attrib.diffuse", diffuse);
        this->shader->setVec3("pointlights[" + std::to_string(slot) + "].attrib.specular", specular);
        this->shader->setFloat("pointlights[" + std::to_string(slot) + "].attenuation.constant", constant);
        this->shader->setFloat("pointlights[" + std::to_string(slot) + "].attenuation.linear", linear);
        // this->shader->setFloat("pointlights[" + std::to_string(slot) + "].attenuation.quadratic", quadratic);
        update();
    }

    void updateSpotlight(unsigned int slot, glm::vec3 direction, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutoff, float outercutoff) {
        error();

        this->shader->setVec3("spotlights[" + std::to_string(slot) + "].direction", direction);
        this->shader->setVec3("spotlights[" + std::to_string(slot) + "].position", position);
        this->shader->setVec3("spotlights[" + std::to_string(slot) + "].attrib.ambient", ambient);
        this->shader->setVec3("spotlights[" + std::to_string(slot) + "].attrib.diffuse", diffuse);
        this->shader->setVec3("spotlights[" + std::to_string(slot) + "].attrib.specular", specular);
        this->shader->setFloat("spotlights[" + std::to_string(slot) + "].attenuation.constant", constant);
        this->shader->setFloat("spotlights[" + std::to_string(slot) + "].attenuation.linear", linear);
        // this->shader->setFloat("spotlights[" + std::to_string(slot) + "].attenuation.quadratic", quadratic);

        this->shader->setFloat("spotlights[" + std::to_string(slot) + "].cutoff", glm::cos(glm::radians(cutoff)));
        this->shader->setFloat("spotlights[" + std::to_string(slot) + "].outer_cutoff", glm::cos(glm::radians(outercutoff)));
    }
};
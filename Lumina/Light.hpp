#pragma once

#include <string>
#include <iostream>

#include "Shader.hpp"
#include "glm/glm.hpp"

class Light {
    const std::string error_msg = "ERROR::Shader:: invalid Shader address.";
    
public:
    // number of the lights
    unsigned int num_Dirlight;
    unsigned int num_Pointlight;
    unsigned int num_Spotlight;
    Shader *shader;

    // init
    Light(Shader* aShader) {
        this->num_Dirlight = 0;
        this->num_Pointlight = 0;
        this->num_Spotlight = 0;
        this->shader = aShader;
    }

    void error() const;
    void update() const;
    void updateDirlight(unsigned int slot, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
    void updatePointlight(unsigned int slot, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
    void updateSpotlight(unsigned int slot, glm::vec3 direction, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutoff, float outercutoff);
};

void Light::error() const {
    if(!this->shader)
        std::cout << error_msg << std::endl;
}

void Light::update() const {
    this->shader->setInt("num_dirlight", num_Dirlight);
    this->shader->setInt("num_pointlight", num_Pointlight);
    this->shader->setInt("num_spotlight", num_Spotlight);
}

void Light::updateDirlight(unsigned int slot, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
    error();
    this->shader->setVec3("dirlights[" + std::to_string(slot) + "].direction", direction);
    this->shader->setVec3("dirlights[" + std::to_string(slot) + "].ambient", ambient);
    this->shader->setVec3("dirlights[" + std::to_string(slot) + "].diffuse", diffuse);
    this->shader->setVec3("dirlights[" + std::to_string(slot) + "].specular", specular);
    update();
}

void Light::updatePointlight(unsigned int slot, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic) {
    error();
    this->shader->setVec3("pointlights[" + std::to_string(slot) + "].position", position);
    this->shader->setVec3("pointlights[" + std::to_string(slot) + "].ambient", ambient);
    this->shader->setVec3("pointlights[" + std::to_string(slot) + "].diffuse", diffuse);
    this->shader->setVec3("pointlights[" + std::to_string(slot) + "].specular", specular);
    this->shader->setFloat("pointlights[" + std::to_string(slot) + "].constant", constant);
    this->shader->setFloat("pointlights[" + std::to_string(slot) + "].linear", linear);
    this->shader->setFloat("pointlights[" + std::to_string(slot) + "].quadratic", quadratic);
    update();
}

void Light::updateSpotlight(unsigned int slot, glm::vec3 direction, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutoff, float outercutoff) {
    error();
    this->shader->setVec3("spotlights[" + std::to_string(slot) + "].direction", direction);
    this->shader->setVec3("spotlights[" + std::to_string(slot) + "].position", position);
    this->shader->setVec3("spotlights[" + std::to_string(slot) + "].ambient", ambient);
    this->shader->setVec3("spotlights[" + std::to_string(slot) + "].diffuse", diffuse);
    this->shader->setVec3("spotlights[" + std::to_string(slot) + "].specular", specular);
    this->shader->setFloat("spotlights[" + std::to_string(slot) + "].constant", constant);
    this->shader->setFloat("spotlights[" + std::to_string(slot) + "].linear", linear);
    this->shader->setFloat("spotlights[" + std::to_string(slot) + "].quadratic", quadratic);

    this->shader->setFloat("spotlights[" + std::to_string(slot) +"].cutoff", glm::cos(glm::radians(cutoff)));
    this->shader->setFloat("spotlights[" + std::to_string(slot) +"].outer_cutoff", glm::cos(glm::radians(outercutoff)));
    update();
}
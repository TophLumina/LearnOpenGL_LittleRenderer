#pragma once

#include <string>
#include <iostream>

#include "../Shader.hpp"

typedef struct Attenuation
{
    Attenuation() : constant(0), linear(0), quadratic(0){};
    Attenuation(float a, float b, float c) : constant(a), linear(b), quadratic(c){};
    float constant;
    float linear;
    float quadratic;
} Attenuation;

typedef struct LightAttrib
{
    LightAttrib() : ambient(glm::vec3(0)), diffuse(glm::vec3(0)), specular(glm::vec3(0)){};
    LightAttrib(glm::vec3 a, glm::vec3 b, glm::vec3 c) : ambient(a), diffuse(b), specular(c){};
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
} LightAttrib;


const std::string shader_error_msg = "ERROR::Lights::Shader:: invalid Shader address.";

class Tools {
public:
    static void ShaderCheck(Shader *shader) {
        if (!shader)
            std::cout << shader_error_msg << std::endl;
    }
};
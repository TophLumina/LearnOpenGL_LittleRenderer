#version 330 core

// const int Diffuse_MAX = 4;
// const int Specular_MAX = 2;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    // sampler2D texture_diffuse3;
    // sampler2D texture_diffuse4;
    sampler2D texture_specular1;
    // sampler2D texture_specular2;
};

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

out vec4 FragColor;

uniform Material material;

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);
    result += vec3(texture(material.texture_diffuse1, texCoords));
    // result += vec3(texture(material.texture_diffuse2, texCoords));

    FragColor = vec4(result, 1.0);
}
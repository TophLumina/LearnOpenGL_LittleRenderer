# version 330 core

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
in vec3 texCoords;

uniform Material material;

out vec4 FragColor;

void main {
    vec3 result(0.0, 0.0, 0.0);

    FragColor = vec4(result, 1.0);
}
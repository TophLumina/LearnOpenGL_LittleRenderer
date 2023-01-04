#version 330 core

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
uniform bool DepthVisualize;

float DepthLinearize(float depth, float znear, float zfar);

void main() {
    vec3 result = vec3(0.0, 0.0, 0.0);

    result += DepthVisualize ? vec3(DepthLinearize(gl_FragCoord.z, 0.1, 100.0)) : vec3(texture(material.texture_diffuse1, texCoords));
    FragColor = vec4(result ,1.0);
}

//Transforming the Z-Buffer back in linear format
float DepthLinearize(float depth, float znear, float zfar) {
    depth = depth * 2 - 1.0;
    return (2.0 * znear * zfar) / (zfar + znear - depth * (zfar - znear));
}
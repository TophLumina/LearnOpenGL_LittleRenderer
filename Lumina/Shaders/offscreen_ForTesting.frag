#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
} fs_in;

uniform sampler2D DepthMap;

void main() {
    float value = texture(DepthMap, fs_in.texCoords).r;
    FragColor = vec4(vec3(value), 1.0);
}
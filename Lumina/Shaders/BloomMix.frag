#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
} fs_in;

uniform sampler2D color;
uniform sampler2D bloomblur;

void main() {
    vec3 hdrcolor = texture(color, fs_in.texCoords).rgb;
    vec3 bloomcolor = texture(bloomblur, fs_in.texCoords).rgb;

    hdrcolor += 0.35 * bloomcolor; // Simple Blending
    FragColor = vec4(hdrcolor, 1.0);
}
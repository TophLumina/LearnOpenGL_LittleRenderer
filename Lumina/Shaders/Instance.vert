#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec3 aCol;

out VS_OUT {
    vec3 col;
} vs_out;

uniform vec2 offsets[100];

void main() {
    vec2 offset = offsets[gl_InstanceID];
    vs_out.col = aCol;

    gl_Position = vec4(aPosition + offset, 0.0, 1.0);
}
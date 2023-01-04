#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aCol;

out VS_OUT {
    vec3 col;
} vs_out;

void main() {
    vs_out.col = aCol;
    gl_Position = vec4(aPos, 0., 1.0);
}
#version 330 core

layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aOffset;

out VS_OUT {
    vec3 col;
} vs_out;

void main() {
    vs_out.col = aCol;

    gl_Position = vec4(aPosition + aOffset, 0.0, 1.0);
}
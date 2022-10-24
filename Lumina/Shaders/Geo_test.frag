#version 330 core

out vec4 FragColor;

in vec3 fs_col;

void main() {
    FragColor = vec4(fs_col, 1.0);
}
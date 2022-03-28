#version 330 core
layout (location = 0) in vec3 aPosition;
// layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrices;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main() {
    gl_Position = projection * view * instanceMatrices * vec4(aPosition, 1.0);
}

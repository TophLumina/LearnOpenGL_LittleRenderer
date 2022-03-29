#version 330 core
layout (location = 0) in vec3 aPosition;
// layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instanceMatrices;
layout (location = 7) in vec3 aCol;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out VS_OUT {
    vec3 color;
} vs_out;

void main() {
    vs_out.color = aCol;

    gl_Position = projection * view * instanceMatrices * vec4(aPosition, 1.0);
}

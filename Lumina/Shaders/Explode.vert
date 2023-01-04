#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

out VS_OUT {
    vec3 normal;
    vec3 fragPos;
    vec2 texCoords;
} vs_out;

void main() {
    vs_out.normal = mat3(transpose(inverse(model * view))) * aNormal;
    vs_out.fragPos = vec3(view * model * vec4(aPosition, 1.0));
    vs_out.texCoords = aTexCoords;

    gl_Position = projection * vec4(vs_out.fragPos, 1.0);
}
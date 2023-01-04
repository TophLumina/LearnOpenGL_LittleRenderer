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
} vs_out;

void main() {
    mat3 TransNormal = mat3(transpose(inverse(model * view)));
    vs_out.normal = normalize(vec3(projection * vec4(TransNormal * aNormal, 0.0)));

    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
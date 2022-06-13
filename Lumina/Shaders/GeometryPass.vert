# version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
    vec3 viewpos;
};

out VS_OUT {
    vec3 normal;
    vec3 fragpos;
    vec2 texCoords;
} vs_out;

void main() {
    vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.fragpos = vec3(model * vec4(aPosition, 1.0));
    vs_out.texCoords = aTexCoords;

    gl_Position = projection * view * vec4(vs_out.fragpos, 1.0);
}
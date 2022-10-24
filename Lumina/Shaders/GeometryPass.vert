#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBiTangent;

uniform mat4 model;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
    vec3 viewpos;
};

out VS_OUT {
    vec3 fragpos_world;
    vec3 fragpos_view;
    vec3 normal;
    vec3 normal_view;
    vec2 texCoords;
} vs_out;

void main() {
    vec4 temp = model * vec4(aPosition, 1.0);
    vs_out.fragpos_world = vec3(temp);
    temp = view * temp;
    vs_out.fragpos_view = vec3(temp);

    vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.normal_view = mat3(transpose(inverse(model * view))) * aNormal;
    
    vs_out.texCoords = aTexCoords;

    gl_Position = projection * temp;
}
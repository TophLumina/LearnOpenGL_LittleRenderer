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
    vec3 normal; // normal
    vec3 fragpos;
    vec2 texCoords;
    vec3 viewPos;

    // Tangent Space to World Space Transform Matrix
    mat3 TBN;
    mat3 iTBN;
} vs_out;

void main() {
    vs_out.normal = mat3(transpose(inverse(model))) * aNormal;
    vs_out.fragpos = vec3(model * vec4(aPosition, 1.0));
    vs_out.texCoords = aTexCoords;
    vs_out.viewPos = viewpos;

    // TBN Matrix <view_space>
    vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(model * vec4(aBiTangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
    vs_out.TBN = mat3(T, B, N);
    vs_out.iTBN = transpose(vs_out.TBN);

    gl_Position = projection * view * vec4(vs_out.fragpos, 1.0);
}
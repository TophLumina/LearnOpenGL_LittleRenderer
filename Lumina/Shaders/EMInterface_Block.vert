#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 model;

layout (std140) uniform Matrices {
                        // Standard Offset      Aligned Offset
    mat4 view;          // 16                   0   [0]
                        // 16                   16  [1]
                        // 16                   32  [2]
                        // 16                   48  [3]

    mat4 projection;    // 16                   64  [0]
                        // 16                   80  [1]
                        // 16                   96  [2]
                        // 16                   112 [3]
                        // intotal -- 112 + 16 = 128
};

out VS_OUT {
    vec3 normal;
    vec3 fragPos;
    vec2 texCoords;
} vs_out;

void main() {
    // Interface_Block Usage
    vs_out.normal = mat3(transpose(inverse(model * view))) * aNormal;
    vs_out.fragPos = vec3(view * model * vec4(aPosition, 1.0));
    vs_out.texCoords = aTexCoords;

    gl_Position = projection * vec4(vs_out.fragPos, 1.0);
}
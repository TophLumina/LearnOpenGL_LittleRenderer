#version 330 core
layout(location = 0) in vec3 aPosition;

out VS_OUT {
    vec3 textureDir;
} vs_out;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
};

void main() {
    gl_Position = projection * view * vec4(aPosition, 1.0);
    vs_out.textureDir = aPosition;
    gl_Position = gl_Position.xyzz;
}
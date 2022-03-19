# version 330 core
layout(location = 0) in vec3 aPosition;

out vec3 textureDir;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPosition, 1.0);
    textureDir = aPosition;
    // Use PRE_DEPTH_TEST to discard all invisible pixsels
    gl_Position = gl_Position.xyzz;
}

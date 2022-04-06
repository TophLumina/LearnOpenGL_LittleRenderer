# version 330 core
// This Requires all Other Shader should set their Position Attrib Layer to ZERO!
layout (location = 0) in vec3 aPosition;
layout (location = 3) in mat4 instanceMatrices;

uniform bool useInstance;

uniform mat4 model;
uniform mat4 LightSpaceTransform;

void main() {
    gl_Position = LightSpaceTransform * (useInstance ? instanceMatrices : model) * vec4(aPosition, 1.0);
}
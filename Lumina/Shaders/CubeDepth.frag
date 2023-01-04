#version 330 core

in vec4 FragPos;

uniform vec3 LightPos;
uniform float Far;

void main() {
    // Caculate Depth Manually
    float light2frag = length(FragPos.xyz - LightPos);

    // Trans to Depth
    light2frag /= Far;

    // Write FragDepth <This also disable the Pre-DepthTest Function of GPU>
    gl_FragDepth = light2frag;
}
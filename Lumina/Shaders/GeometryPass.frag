# version 330 core

layout (location = 0) out vec4 gPosition_World;
layout (location = 1) out vec4 gPosition_View;
// gPosition = (x, y, z, linearized_depth)
layout (location = 2) out vec4 gNormal_World;
layout (location = 3) out vec4 gNormal_View;
layout (location = 4) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 fragpos_world;
    vec3 fragpos_view;
    vec3 normal;
    vec3 normal_view;
    vec2 texCoords;
} fs_in;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
};

uniform Material material;
uniform float z_near;
uniform float z_far;

float linearize_depth(float depth) {
    depth = depth * 2.0 - 1.0;
    return (2.0 * z_far * z_near) / (z_far + z_near - depth * (z_far - z_near));
}

void main() {
    // gPosition_World = vec4(fs_in.fragpos_world, linearize_depth(gl_FragCoord.z));
    // gPosition_View = vec4(fs_in.fragpos_view, linearize_depth(gl_FragCoord.z));
    gPosition_World = vec4(fs_in.fragpos_world, 1.0);
    gPosition_View = vec4(fs_in.fragpos_view, 1.0);
    gNormal_World = vec4(normalize(fs_in.normal), 1.0);
    gNormal_View = vec4(normalize(fs_in.normal_view), 1.0);
    gAlbedoSpec.rgb = texture(material.texture_diffuse1, fs_in.texCoords).rgb;
    // gAlbedoSpec.a = texture(material.texture_specular1, fs_in.texCoords).r;
    gAlbedoSpec.a = 1.0;
}
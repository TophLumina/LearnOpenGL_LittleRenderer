# version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in VS_OUT {
    vec3 normal;
    vec3 fragpos;
    vec2 texCoords;
} fs_in;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
};

uniform Material material;

void main() {
    // <World Space>
    gPosition = fs_in.fragpos;
    gNormal = normalize(fs_in.normal);
    gAlbedoSpec.rgb = texture(material.texture_diffuse1, fs_in.texCoords).rgb;
    // gAlbedoSpec.a = texture(material.texture_specular1, fs_in.texCoords).r;
    gAlbedoSpec.a = 1.0;
}
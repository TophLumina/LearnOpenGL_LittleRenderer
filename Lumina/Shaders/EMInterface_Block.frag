#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
};

in VS_OUT {
    vec3 normal;
    vec3 fragPos;
    vec2 texCoords;
} fs_in;

out vec4 FragColor;

uniform Material material;

void main() {
    vec4 col = texture(material.texture_diffuse1, fs_in.texCoords);

    if(col.a < 0.05)
        discard;
    
    FragColor = col;
}
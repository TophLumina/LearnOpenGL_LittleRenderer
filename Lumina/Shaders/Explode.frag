#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
};

in vec2 texCoords;

out vec4 FragColor;

uniform Material material;

void main() {
    vec4 col = texture(material.texture_diffuse1, texCoords);

    if(col.a < 0.05)
        discard;
    
    FragColor = col;
}
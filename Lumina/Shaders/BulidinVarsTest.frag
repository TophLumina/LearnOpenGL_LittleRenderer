# version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
};

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

out vec4 FragColor;

uniform Material material;
uniform samplerCube skybox;

// const float refractRatio = 1 / 1.33;

void main() {
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
    
    // Need to Disable Face_Culling first
    if(gl_FrontFacing)
        col += texture(material.texture_diffuse1, texCoords);
    else
        col += vec4(1.0, 0.67, 0.13, 1.0);

    if(col.a < 0.005)
        discard;
    
    FragColor = col;
}
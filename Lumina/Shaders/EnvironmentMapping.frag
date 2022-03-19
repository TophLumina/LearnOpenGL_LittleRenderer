# version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    // sampler2D texture_diffuse3;
    // sampler2D texture_diffuse4;
    sampler2D texture_specular1;
    // sampler2D texture_specular2;
};

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;

out vec4 FragColor;

uniform Material material;
uniform samplerCube skybox;

const float refractRatio = 1 / 1.33;

void main() {
    vec4 col = vec4(0.0, 0.0, 0.0, 0.0);
    col += texture(material.texture_diffuse1, texCoords);

    // Sampler slots need refinement
    if(col.a < 0.1)
        discard;

    vec3 viewDir = normalize(-fragPos);

    // Reflection and Refraction

    // vec3 reflectDir = reflect(-viewDir, normalize(normal));
    vec3 refractDir = refract(-viewDir, normalize(normal), refractRatio);

    // FragColor = vec4(texture(skybox, reflectDir).rgb, 1.0);
    FragColor = vec4(texture(skybox, refractDir).rgb, 1.0);
}
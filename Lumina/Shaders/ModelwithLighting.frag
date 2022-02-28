# version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    // sampler2D texture_diffuse3;
    // sampler2D texture_diffuse4;
    sampler2D texture_specular1;
    // sampler2D texture_specular2;
};

uniform Material material;

struct Dirlight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutoff;
    float outer_cutoff;
};

in vec3 normal;
in vec3 fragPos;
in vec3 texCoords;

#define POINT_LIGHTS_LIMITATION 16
#define OTHER_LIMITATION 2
const float shininess = 32.0;

uniform Dirlight dirlights[OTHER_LIMITATION];
uniform PointLight pointlights[POINT_LIGHTS_LIMITATION];
uniform SpotLight spotlights[OTHER_LIMITATION];
uniform int num_dirlight;
uniform int num_pointlight;
uniform int num_spotlight;

vec3 CalculateDirlight(Dirlight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointlight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotlight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

out vec4 FragColor;

void main {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(-fragPos);

    vec3 result(0.0, 0.0, 0.0);

    for(int i = 0; i < num_dirlight; ++i)
        result += CalculateDirlight(dirlights[i], norm, viewDir);
    
    for(int i = 0; i < num_pointlight; ++i)
        result += CalculatePointlight(pointlights[i], norm, fragPos, viewDir);

    for(int i = 0; i < num_spotlight; ++i)
        result += CalculateSpotlight(spotlights[i], norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}
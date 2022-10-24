#version 330 core

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

    float constant;
    float linear;
    float quadratic;

    float cutoff;
    float outer_cutoff;
};

in vec3 normal;
in vec3 fragPos;
in vec2 texCoords;
in mat4 out_view;

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
vec3 ComicLikeDirLight(Dirlight light, vec3 normal, vec3 viewDir);

out vec4 FragColor;

void main() {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(-fragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);

    // for(int i = 0; i < num_dirlight; ++i)
    //     result += CalculateDirlight(dirlights[i], norm, viewDir);
    
    // for(int i = 0; i < num_pointlight; ++i)
    //     result += CalculatePointlight(pointlights[i], norm, fragPos, viewDir);

    // for(int i = 0; i < num_spotlight; ++i)
    //     result += CalculateSpotlight(spotlights[i], norm, fragPos, viewDir);

    for (int i = 0; i < num_dirlight; ++i)
        result += ComicLikeDirLight(dirlights[i], norm, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalculateDirlight(Dirlight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-vec3(mat4(mat3(out_view)) * vec4(light.direction, 1.0)));
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1,texCoords));

    return (ambient + diffuse + specular);
}

vec3 CalculatePointlight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
    float distance = length(fragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));

    return attenuation * (ambient + diffuse + specular);
}

vec3 CalculateSpotlight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(lightDir, viewDir), 0.0), shininess);
    float distance = length(fragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    float theta = dot(light.direction, -lightDir);
    float epsilon = light.cutoff - light.outer_cutoff;
    float intensity = clamp((theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec *vec3(texture(material.texture_specular1, texCoords));

    return attenuation * intensity * ((theta > light.outer_cutoff) ? ambient + diffuse + specular : ambient);
}

vec3 ComicLikeDirLight(Dirlight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-vec3(mat4(mat3(out_view)) * vec4(light.direction, 1.0)));
    bool bright = dot(lightDir, normal) > 0.2 ? true : false;

    vec3 ambient = 0.4 * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = 0.6 * vec3(texture(material.texture_diffuse1, texCoords));

    return bright ? ambient + diffuse : ambient;
}
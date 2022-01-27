# version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

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
    float outercutoff;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

#define POINT_LIGHTS_LIMITATION 16
#define OTHER_LIMITATION 2

uniform Material material;
uniform Dirlight dirlights[OTHER_LIMITATION];
uniform PointLight pointlights[POINT_LIGHTS_LIMITATION];
uniform SpotLight spotlights[OTHER_LIMITATION];
uniform int num_dirlight;
uniform int num_pointlight;
uniform int num_spotlight;

//func for calculating lighting
vec3 CalculateDirlight(Dirlight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointlight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateSpotlight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main() {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(-fragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < num_dirlight; ++i)
        result += CalculateDirlight(dirlights[i], norm, viewDir);

    for(int i = 0; i < num_pointlight; ++i)
        result += CalculatePointlight(pointlights[i], norm, fragPos, viewDir);

    for(int i = 0; i < num_spotlight; ++i)
        result += CalculateSpotlight(spotlights[i], norm, fragPos, viewDir);

    FragColor = vec4(result, 1.0);
}

vec3 CalculateDirlight(Dirlight light, vec3 normal, vec3 viewDir) {
    //diffuse
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);

    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, texCoords));
    vec3 specular = spec * light.specular * vec3(texture(material.specular, texCoords));
    return (ambient + diffuse + specular);
}

vec3 CalculatePointlight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
    //Attenuation
    float distance = length(fragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, texCoords));
    vec3 specular = spec * light.specular * vec3(texture(material.specular, texCoords));

    return attenuation * (ambient + diffuse + specular);
}

vec3 CalculateSpotlight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);

    float theta = dot(light.direction, -lightDir);
    float epsilon = light.cutoff - light.outercutoff;
    float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0, 1.0);

    //Attenuation
    float distance = length(fragPos - light.position);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, texCoords));
    vec3 specular = spec * light.specular * vec3(texture(material.specular, texCoords));

    return intensity * attenuation * ((theta > light.outercutoff) ? ambient + diffuse + specular : ambient);
}
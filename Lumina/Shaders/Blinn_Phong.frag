#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;
    sampler2D texture_specular1;
};

struct LightAttrib {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Attenuation {
    float constant;
    float linear;

    // float quadratic;
};

struct Dirlight {
    vec3 direction;

    LightAttrib attrib;
};

struct PointLight {
    vec3 position;

    LightAttrib attrib;
    Attenuation attenuation;
};

struct SpotLight {
    vec3 position;
    vec3 direction;

    LightAttrib attrib;
    Attenuation attenuation;

    float cutoff;
    float outer_cutoff;
};

in VS_OUT {
    vec3 normal;
    vec3 fragPos;
    vec2 texCoords;
    mat4 view;
} fs_in;

uniform Material material;

// Light Config
const int POINT_LIGHTS_LIMITATION = 16;
const int OTHER_LIMITATION = 2;
const float shininess = 32.0;

uniform Dirlight dirlights[OTHER_LIMITATION];
uniform PointLight pointlights[POINT_LIGHTS_LIMITATION];
uniform SpotLight spotlights[OTHER_LIMITATION];

uniform int num_dirlight;
uniform int num_pointlight;
uniform int num_spotlight;

bool FragmentVisibility();
vec3 CalculateDirlight(Dirlight light, vec3 normal, vec3 viewDir);
vec3 FlatDirlight(Dirlight light, vec3 normal, vec3 viewDir);
// vec3 CalculatePointlight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
// vec3 CalculateSpotlight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

uniform bool GammaCorrection;

out vec4 FragColor;

void main() {
    if(!FragmentVisibility())
        discard;

    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(-fs_in.fragPos);
    vec3 result = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < num_dirlight; ++i)
        // result += CalculateDirlight(dirlights[i], norm, viewDir);
        result += FlatDirlight(dirlights[i], norm, viewDir);

    FragColor = vec4(result, 1.0);
}

bool FragmentVisibility() {
    return texture(material.texture_diffuse1, fs_in.texCoords).a > 0.05;
}

vec3 CalculateDirlight(Dirlight light, vec3 normal, vec3 viewDir) {
    vec3 lightDir = normalize(-vec3(mat4(mat3(fs_in.view)) * vec4(light.direction, 1.0)));
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 half_vec = normalize(lightDir + viewDir);
    float spec = pow(max(dot(half_vec, normal), 0.0), shininess);

    vec3 ambient = light.attrib.ambient * vec3(texture(material.texture_diffuse1, fs_in.texCoords));
    vec3 diffuse = light.attrib.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.texCoords));
    vec3 specular = light.attrib.specular * spec * vec3(texture(material.texture_specular1, fs_in.texCoords));

    return (ambient + diffuse + specular);
}

vec3 FlatDirlight(Dirlight light, vec3 normal, vec3 viewDir) {
    // for lightDir, we want to keep its position but change its direction
    vec3 lightDir = normalize(-vec3(mat4(mat3(fs_in.view)) * vec4(light.direction, 1.0)));
    bool isbright = dot(lightDir, normal) > 0;
    vec3 half_vec = normalize(lightDir + viewDir);
    bool ishighlight = pow(max(dot(half_vec, normal), 0.0), shininess) > 0.95;

    vec3 ambient = light.attrib.ambient * vec3(texture(material.texture_diffuse1, fs_in.texCoords));
    vec3 diffuse = light.attrib.diffuse * vec3(texture(material.texture_diffuse1, fs_in.texCoords));
    vec3 specular = light.attrib.specular * vec3(texture(material.texture_specular1, fs_in.texCoords));

    return isbright ? (ishighlight ? ambient + diffuse + specular : ambient + diffuse) : ambient;
}
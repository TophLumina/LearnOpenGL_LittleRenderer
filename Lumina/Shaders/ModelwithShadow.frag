# version 330 core

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
    sampler2D shadowmap;

    LightAttrib attrib;
};

struct PointLight {
    vec3 position;
    samplerCube shadowmap;

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

const int POINT_LIGHTS_LIMITATION = 8;
const int OTHER_LIMITATION = 2;

in VS_OUT {
    vec3 normal;
    vec3 viewspace_fragPos;
    vec2 texCoords;
    mat4 view;
    // vec4 lightspace_fragPos;

    flat int num_dirlight;
    flat int num_pointlight;
    flat int num_spotlight;

    vec4 dirlight_fragPos[OTHER_LIMITATION];
    vec4 pointlight_fragPos[POINT_LIGHTS_LIMITATION];
} fs_in;

uniform Material material;

const float shininess = 32.0;

uniform Dirlight dirlights[OTHER_LIMITATION];
uniform PointLight pointlights[POINT_LIGHTS_LIMITATION];
uniform SpotLight spotlights[OTHER_LIMITATION];

// Shadow Map Texture(s)
uniform sampler2D dirShadowMap[OTHER_LIMITATION];
uniform samplerCube pointShadowMap[POINT_LIGHTS_LIMITATION];

bool FragmentVisibility();
float ShadowFactor(Dirlight, vec4);

uniform bool GammaCorrection;

out vec4 FragColor;

void main() {
    if(!FragmentVisibility())
        discard;

    vec3 norm = normalize(fs_in.normal);
    vec3 viewDir = normalize(-fs_in.viewspace_fragPos);
    vec3 result = vec3(0.0, 0.0, 0.0);

    // Test Code
    for (int i = 0; i < fs_in.num_dirlight; ++i)
        result += ShadowFactor(dirlights[i], fs_in.dirlight_fragPos[i]);
    FragColor = vec4(result, 1.0);

    // FragColor = vec4(texture(material.texture_diffuse1, fs_in.texCoords));
}


bool FragmentVisibility() {
    return texture(material.texture_diffuse1, fs_in.texCoords).a > 0.05;
}

float ShadowFactor(Dirlight light, vec4 light_frag_pos) {
    // Perspective Projection
    vec3 projCoords = light_frag_pos.xyz / light_frag_pos.w;
    // Depth start from 0 to 1
    projCoords = projCoords * 0.5 + 0.5;

    // Refinements
    float adjust = max(0.01 * (1.0 - dot(normalize(fs_in.normal), normalize(-vec3(mat4(mat3(fs_in.view)) * vec4(light.direction, 1.0))))), 0.005);

    float CurrentDepth = projCoords.z;
    float shadow = 0.0;

    // 25 * Multi Sampling
    vec2 pixeloffset = 0.3 / textureSize(light.shadowmap, 0);
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            float subdepth = texture(light.shadowmap, projCoords.xy + pixeloffset * vec2(x, y)).r;
            shadow += CurrentDepth > subdepth + adjust ? 1.0 : 0.0;
        }
    }

    shadow /= 25;

    if(CurrentDepth > 1.0)
        return 1.0;

    return 1.0 - shadow;
}
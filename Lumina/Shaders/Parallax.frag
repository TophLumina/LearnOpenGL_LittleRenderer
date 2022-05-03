# version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_diffuse2;

    sampler2D texture_specular1;

    sampler2D texture_normal1;
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
    float far;

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
    vec3 fragpos;
    vec2 texCoords;
    vec3 viewPos;

    flat int num_dirlight;
    flat int num_pointlight;
    flat int num_spotlight;

    vec4 dirlight_fragPos[OTHER_LIMITATION];

    mat3 TBN;
    mat3 iTBN;
} fs_in;

uniform Material material;

const float shininess = 32.0;

uniform Dirlight dirlights[OTHER_LIMITATION];
uniform PointLight pointlights[POINT_LIGHTS_LIMITATION];
uniform SpotLight spotlights[OTHER_LIMITATION];

vec2 ParallaxMapping(vec2 coords, vec3 viewdir);    // For ParallaxMapping (of course)
bool FragmentVisibility(vec2 coords);
bool IsBright(vec3 lightdir, vec3 norm);
float DepthAdjustment(vec3 lightdir);
float ShadowFactor(Dirlight light, vec4 light_frag_pos);
float ShadowFactor(PointLight light);

uniform bool GammaCorrection;

out vec4 FragColor;

uniform sampler2D parallaxmap;

void main() {
    vec3 viewDir = normalize(fs_in.viewPos -fs_in.fragpos);

    // Parallax Mapping
    vec2 coord = ParallaxMapping(fs_in.texCoords, viewDir);
    if(coord.x > 1.0 || coord.x < 0.0 || coord.y > 1.0 || coord.y < 0.0)
        discard;

    // vec2 coord = fs_in.texCoords

    if(!FragmentVisibility(coord))
        discard;

    // vec3 norm = normalize(fs_in.normal);
    vec3 norm = normalize(fs_in.TBN * normalize(texture(material.texture_normal1, coord).rgb * 2.0 - 1.0));
    vec3 result = vec3(0.0, 0.0, 0.0);

    // float imp = IsBright(-dirlights[0].direction, norm) ? ShadowFactor(dirlights[0], fs_in.dirlight_fragPos[0]) : 0.0;
    float imp = IsBright(pointlights[0].position - fs_in.fragpos, norm) ? ShadowFactor(pointlights[0]) : 0.0;
    imp = imp * 0.6 + 0.4;

    result += vec3(imp * texture(material.texture_diffuse1, coord));
    // FragColor = vec4(result, 1.0);

    // TEST CODE
    FragColor = vec4(imp * texture(material.texture_diffuse1, coord).rgb, 1.0);
}


vec2 ParallaxMapping(vec2 coords, vec3 viewdir) {
    const float f = 0.01;
    viewdir = normalize(fs_in.iTBN * viewdir);
    float height = 1.0 - texture(parallaxmap, coords).r;
    vec2 offset = viewdir.xy / viewdir.z * height * f;
    return coords - offset;
}

bool FragmentVisibility(vec2 coords) {
    return texture(material.texture_diffuse1, coords).a > 0.05;
}

bool IsBright(vec3 lightdir, vec3 norm) {
    vec3 dir = normalize(lightdir);
    return dot(norm, dir) > 0;
}

float DepthAdjustment(vec3 lightdir) {
    return max(0.005 * (1.0 - max(dot(normalize(fs_in.normal), normalize(lightdir)), 0.0)), 0.001);
}

float ShadowFactor(Dirlight light, vec4 light_frag_pos) {
    // Perspective Projection
    vec3 projCoords = light_frag_pos.xyz / light_frag_pos.w;
    // Depth start from 0 to 1
    projCoords = projCoords * 0.5 + 0.5;

    // Refinements
    float adjust = DepthAdjustment(-light.direction);

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

float ShadowFactor(PointLight light) {
    vec3 Light2Frag = light.position - fs_in.fragpos;

    vec3 FragDir = normalize(Light2Frag);

    float CurrentDepth = length(Light2Frag);

    float adjust = DepthAdjustment(Light2Frag);

    const int samples = 20;

    vec3 offsets[samples] = vec3[] (
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    float bias = 0.0001;

    float shadow = 0.0;

    for (int i = 0; i < samples; ++i) {
        float subStoppingDepth = texture(light.shadowmap, FragDir + offsets[i] * bias).r;
        subStoppingDepth *= light.far;
        shadow += CurrentDepth > subStoppingDepth + adjust ? 1.0 : 0.0;
    }

    shadow /= samples;

    if(CurrentDepth > light.far)
        return 0.0;
    
    return 1.0 - shadow;
}
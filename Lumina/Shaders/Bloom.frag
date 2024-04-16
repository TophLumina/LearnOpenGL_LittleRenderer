#version 330 core

// MRT
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

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

    // fragpos in light space
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
float Brightness(PointLight light, vec3 frag2light);

uniform bool GammaCorrection;

#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586
#define NUM_SAMPLES 32
#define NUM_RINGS 12

// -1 to 1
float rand_1to1(float seed) {
    return fract(sin(seed) * 43758.5453);
}

// 0 to 1
float rand_2to1(vec2 seed2) {
    const float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = dot(seed2.xy, vec2(a, b));
    float sn = mod(dt, PI);
    return fract(sin(sn) * c);
}

vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples(vec2 seed) {
    float ANGLE_STEP = float(NUM_RINGS) / float(NUM_SAMPLES) * PI2;
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

    float angle = rand_2to1(seed) * PI2;
    float radius = INV_NUM_SAMPLES;
    float radiusStep = radius;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        poissonDisk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

void uniformDiskSamples(vec2 seed) {
    float randNum = rand_2to1(seed);
    float sampleX = rand_1to1(randNum);
    float sampleY = rand_1to1(sampleX);

    float angle = sampleX * PI2;
    float radius = sqrt(sampleY);

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        poissonDisk[i] = vec2(radius * cos(angle), radius * sin(angle));

        sampleX = rand_1to1(sampleY);
        sampleY = rand_1to1(sampleX);

        angle = sampleX * PI2;
        radius = sqrt(sampleY);
    }
}

const float shadowMapRes = 512.0;

float DepthAdjustment(vec3 lightdir) {
    vec3 norm = normalize(fs_in.normal);
    const float standard = 0.01;
    return standard * (1.0 - abs(dot(norm, normalize(lightdir))));
}

float findBlocker(sampler2D shadowmap, vec4 light_coord, vec3 lightDir) {
    uniformDiskSamples(light_coord.xy);

    float offset = 0.6 / shadowMapRes;
    float sum = 0.0;
    int num = 0;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        float blocker_depth = texture(shadowmap, light_coord.xy + poissonDisk[i] * offset).r;
        if (blocker_depth + DepthAdjustment(lightDir) < light_coord.z) {
            sum += blocker_depth;
            num++;
        }
    }

    if (num == 0)
        return 0.0;

    return sum / (float(num) + EPS); // avoid divide by zero
}

float PCF_adptive(sampler2D shadowmap, vec4 light_coord, vec3 lightDir, float fliter_size) {
    poissonDiskSamples(light_coord.xy); // build sampler disk

    float shadow_factor = 0.0;
    float depth = light_coord.z;
    float offset = 2.0 / shadowMapRes * fliter_size;

    for (int i = 0; i < NUM_SAMPLES; ++i) {
        float blocker_depth = texture(shadowmap, light_coord.xy + poissonDisk[i] * offset).r;
        shadow_factor += ((blocker_depth + DepthAdjustment(lightDir)) < light_coord.z) ? 0.0 : 1.0;
    }

    return shadow_factor / float(NUM_SAMPLES);
}

float PCSS(sampler2D shadowmap, vec4 light_coord, vec3 lightDir) {
    // avgBlockerDepth
    float avgBlockerDepth = findBlocker(shadowmap, light_coord, lightDir);
    if (avgBlockerDepth < EPS)
        return 1.0;
    
    // penumbra
    float fliter_size = 2.0 * (light_coord.z - avgBlockerDepth) / avgBlockerDepth;

    // PCF_adptive
    return PCF_adptive(shadowmap, light_coord, lightDir, fliter_size);
}

void main() {
    vec3 viewDir = normalize(fs_in.viewPos -fs_in.fragpos);

    vec2 coord = fs_in.texCoords;

    vec3 world_norm = normalize(fs_in.normal);
    vec3 tangent_norm = normalize(fs_in.TBN * normalize(texture(material.texture_normal1, coord).rgb * 2.0 - 1.0));

    vec3 result = vec3(0.0, 0.0, 0.0);

    // PCSS
    vec4 light_coord = fs_in.dirlight_fragPos[0];
    // remap to [0, 1]
    light_coord = light_coord / light_coord.w * 0.5 + 0.5;
    float imp = PCSS(dirlights[0].shadowmap, light_coord, -dirlights[0].direction) * 0.95 + 0.05;

    // float imp = dot(normalize(-dirlights[0].direction), world_norm) > 0 ? 1.0: 0.2;

    result += vec3(imp * texture(material.texture_diffuse1, coord));

    FragColor = vec4(result, 1.0);

    // visualize blocker depth
    // float BlockerDep = findBlocker(dirlights[0].shadowmap, light_coord, -dirlights[0].direction);
    // FragColor = vec4(vec3(BlockerDep, BlockerDep, BlockerDep), 1.0);
    
    // Brightness for Bloom
    float bright = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (bright > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

/*
// for Parallax Occlusion Mapping
vec2 ParallaxMapping(vec2 coords, vec3 viewdir) {
    const float f = 0.01;
    viewdir = normalize(fs_in.iTBN * viewdir);

    // const int layers = 25;
    const float minlayers = 8;
    const float maxlayers = 64;
    float layers = mix(maxlayers, minlayers, max(dot(vec3(0.0, 0.0, 1.0), viewdir), 0.0));

    float delta_height = 1.0 / layers;
    vec2 delta_coords = (viewdir.xy / viewdir.z * 1.0 * f) / layers;

    vec2 over_Coords = coords;
    float over_Height = texture(parallaxmap, coords).r;
    float over_LayerHeight = 0.0;

    float pre_Height;

    while(over_Height > over_LayerHeight) {
        pre_Height = texture(parallaxmap, over_Coords).r;

        over_Coords -= delta_coords;
        over_Height = texture(parallaxmap, over_Coords).r;
        over_LayerHeight += delta_height;
    }

    float pre_LayerHeight = over_LayerHeight -= delta_height;
    vec2 pre_Coords = over_Coords += delta_coords;
    float weight = (pre_Height - pre_LayerHeight) / delta_height;
    vec2 finalCoords = mix(pre_Coords, over_Coords, weight);

    return finalCoords;
}
*/

bool FragmentVisibility(vec2 coords) {
    return texture(material.texture_diffuse1, coords).a > 0.05;
}

bool IsBright(vec3 lightdir, vec3 norm) {
    vec3 dir = normalize(lightdir);
    return dot(norm, dir) > 0;
}

float Brightness(PointLight light, vec3 frag2light) {
    return light.attrib.diffuse.r / (1.0 + light.attenuation.constant + light.attenuation.linear * length(frag2light));
}
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
float Brightness(PointLight light, vec3 frag2light);

uniform bool GammaCorrection;

// MRT
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main() {
    vec3 viewDir = normalize(fs_in.viewPos -fs_in.fragpos);

    vec2 coord = fs_in.texCoords;

    vec3 world_norm = normalize(fs_in.normal);
    vec3 tangent_norm = normalize(fs_in.TBN * normalize(texture(material.texture_normal1, coord).rgb * 2.0 - 1.0));

    vec3 result = vec3(0.0, 0.0, 0.0);

    // float imp = IsBright(-dirlights[0].direction, world_norm) ? ShadowFactor(dirlights[0], fs_in.dirlight_fragPos[0]) : 0.0;
    float imp = IsBright(pointlights[0].position - fs_in.fragpos, world_norm) ? ShadowFactor(pointlights[0]) : 0.0;
    imp = imp * Brightness(pointlights[0], (fs_in.fragpos - pointlights[0].position)) * 0.6 + 0.4;

    result += vec3(imp * texture(material.texture_diffuse1, coord));

    // TEST CODE
    // FragColor = vec4(result, 1.0);

    FragColor = vec4(imp * texture(material.texture_diffuse1, coord).rgb, 1.0);
    
    float bright = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (bright > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
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

float Brightness(PointLight light, vec3 frag2light) {
    return light.attrib.diffuse.r / (1.0 + light.attenuation.constant + light.attenuation.linear * length(frag2light));
}
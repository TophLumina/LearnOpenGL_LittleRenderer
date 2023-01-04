# version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec2 texCoords;
} fs_in;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
    vec3 viewpos;
};

const int POINT_LIGHTS_LIMITATION = 8;
const int OTHER_LIMITATION = 2;

struct LightInfo {
    // Amounts
    int num_dirlight;
    int num_pointlight;
    int num_spotlight;

    // Transform Matrices
    mat4 DirLight_Transform[OTHER_LIMITATION];
};

struct LightAttrib {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Attenuation {
    float constant;
    float linear;
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

uniform Dirlight dirlights[OTHER_LIMITATION];
uniform PointLight pointlights[POINT_LIGHTS_LIMITATION];
uniform SpotLight spotlights[OTHER_LIMITATION];

bool IsBright(vec3 lightdir, vec3 norm);
float DepthAdjustment(vec3 lightdir, vec3 norm);
float ShadowFactor(Dirlight light, vec4 light_frag_pos, vec3 norm);
float ShadowFactor(PointLight light, vec3 fragpos, vec3 norm);
float Brightness(PointLight light, vec3 frag2light);

struct GBufferTex {
    sampler2D gPosition_World;  // layer 1
    sampler2D gPosition_View;   // layer 2
    sampler2D gNormal_World;    // layer 3
    sampler2D gNormal_View;     // layer 4
    sampler2D gAlbedoSpec;      // layer 5
};

struct SSAO_Compoent {
    bool apply_SSAO;
    sampler2D SSAOTexture;      // layer 6
};

uniform GBufferTex gbuffertex;
uniform SSAO_Compoent ssao_compoent;

void main() {
    vec3 fragpos = texture(gbuffertex.gPosition_World, fs_in.texCoords).rgb;
    vec3 normal = texture(gbuffertex.gNormal_World, fs_in.texCoords).rgb;
    vec3 albedo = texture(gbuffertex.gAlbedoSpec, fs_in.texCoords).rgb;
    float specular = texture(gbuffertex.gAlbedoSpec, fs_in.texCoords).a;

    vec3 viewDir = normalize(viewpos - fragpos);
    vec3 norm = normalize(normal);

    vec3 result = vec3(0.0, 0.0, 0.0);

    // for (int i = 0; i < lightinfo.num_dirlight; ++i)
    //     dirlight_fragPos[i] = lightinfo.DirLight_Transform[i] * vec4(fragpos, 1.0);
    // float imp = IsBright(-dirlights[0].direction, norm) ? ShadowFactor(dirlights[0], dirlight_fragPos[0], norm) : 0.0;
    float imp_diff = IsBright(pointlights[0].position - fragpos, norm) ? ShadowFactor(pointlights[0], fragpos, norm) : 0.0;
    float ambient_occlusion = texture(ssao_compoent.SSAOTexture, fs_in.texCoords).r;
    float imp_ambi = 1.0 * (ssao_compoent.apply_SSAO ? ambient_occlusion : 1.0);
    float imp = imp_diff * Brightness(pointlights[0], (fragpos - pointlights[0].position)) * 0 + imp_ambi;
    // float imp = imp_diff * Brightness(pointlights[0], (fragpos - pointlights[0].position)) * 0.6 + imp_ambi;

    result += imp * albedo;

    // FragColor = vec4(result, 1.0);
    FragColor = vec4(result, 1.0);

    float bright = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (bright > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

bool IsBright(vec3 lightdir, vec3 norm) {
    vec3 dir = normalize(lightdir);
    return dot(norm, dir) > 0;
}

float DepthAdjustment(vec3 lightdir, vec3 norm) {
    return max(0.005 * (1.0 - max(dot(normalize(norm), normalize(lightdir)), 0.0)), 0.001);
}

float ShadowFactor(Dirlight light, vec4 light_frag_pos, vec3 norm) {
    // Perspective Projection
    vec3 projCoords = light_frag_pos.xyz / light_frag_pos.w;
    // Depth start from 0 to 1
    projCoords = projCoords * 0.5 + 0.5;

    // Refinements
    float adjust = DepthAdjustment(-light.direction, norm);

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

float ShadowFactor(PointLight light, vec3 fragpos, vec3 norm) {
    vec3 Light2Frag = light.position - fragpos;

    vec3 FragDir = normalize(Light2Frag);

    float CurrentDepth = length(Light2Frag);

    float adjust = DepthAdjustment(Light2Frag, norm);

    const int samples = 20;

    vec3 offsets[samples] = vec3[] (
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    // Adaptive Bias
    float bias = (1.0 + length(viewpos - fragpos)) / 25.0;

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

float Frenel(vec3 norm, vec3 view_dir) {
    return pow(1.0 - dot(norm, view_dir), 4.0);
}
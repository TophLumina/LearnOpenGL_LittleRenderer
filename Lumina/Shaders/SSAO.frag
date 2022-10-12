# version 330 core

// <--IMPORTANT--> Usually Unchaged
const int SSAO_KERNAL_SIZE = 64;
const int SSAO_NOISE_SIZE = 4;
const float SAMPLE_RADIUS_OFFSET = 1.35;

in VS_OUT {
    vec2 texCoords;
} fs_in;

out float FragColor;

layout (std140) uniform Matrices {
    mat4 view;
    mat4 projection;
    vec3 viewpos;
};

uniform sampler2D SSAONoise;
uniform sampler2D gPosition_View;
uniform sampler2D gNormal_View;
uniform vec3 samplers[SSAO_KERNAL_SIZE];
uniform int SRC_Width;
uniform int SRC_Height;

vec2 NOISE_SCALE = vec2(SRC_Width/SSAO_NOISE_SIZE, SRC_Height/SSAO_NOISE_SIZE);

void main() {
    vec3 fragpos = texture(gPosition_View, fs_in.texCoords).rgb;
    vec3 normal = texture(gNormal_View, fs_in.texCoords).rgb;
    vec3 noise_vec = texture(SSAONoise, fs_in.texCoords * NOISE_SCALE).rgb;

    vec3 tangent = normalize(noise_vec - normal * dot(noise_vec, normal));
    vec3 bitangent = cross(tangent, normal);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;

    for(int i = 0; i < SSAO_KERNAL_SIZE; ++i) {
        vec3 sample = TBN * samplers[i]; // tangent space -> view space
        sample = fragpos + sample * SAMPLE_RADIUS_OFFSET; // sampling

        vec4 sample_coords = vec4(sample, 1.0);
        sample_coords = projection * sample_coords; // view space -> clip space
        sample_coords.xyz /= sample_coords.w;
        sample_coords.xyz = sample_coords.xyz * 0.5 + 0.5;// -> NCD

        float sample_depth = texture(gPosition_View, sample_coords.xy).w;
        float smooth_occlusion_factor = smoothstep(0.0, 1.0, abs(SAMPLE_RADIUS_OFFSET - sample_depth / sample_coords.w));
        occlusion += sample_depth >= sample_coords.w ? smooth_occlusion_factor : 0.0;
        occlusion -= sample_coords.w == 0 ? 1.0 : 0.0;
    }

    occlusion /= SSAO_KERNAL_SIZE;
    FragColor = occlusion;
}
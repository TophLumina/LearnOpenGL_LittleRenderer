# version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
} fs_in;

uniform sampler2D ScreenTexture;
uniform bool Grayscale;
uniform bool Inversion;

uniform int KernelIndex;

uniform float exposure;

vec4 inversion(vec4 color) {
    vec3 temp = color.rgb;
    return vec4(1.0 - temp, 1.0);
}

vec4 grayscale(vec4 color) {
    float average = (0.2162 * color.r + 0.7152 * color.g + 0.0772 * color.b) / 3;
    return vec4(average, average, average, 1.0);
}

// Kernels
float offset = 2.0 / (textureSize(ScreenTexture, 0).x + textureSize(ScreenTexture, 0).y);

// Gamma Correction
const float Gamma = 2.2;
uniform bool GammaCorrection;

vec2 offsets[9] = vec2[] (
    vec2(-offset, -offset), // left-up
    vec2(-offset, 0.0),     // left
    vec2(-offset, offset),  // left-down
    vec2(0.0, -offset),     // up
    vec2(0.0, 0.0),         //center
    vec2(0.0, offset),      //down
    vec2(offset, -offset),  //right-up
    vec2(offset, 0.0),      //right
    vec2(offset, offset)    //right-down
);

// Kernels
float noeffectkernel[9] = float[] (
    0, 0, 0,
    0, 1, 0,
    0, 0, 0
);

float sharpenkernel[9] = float[] (
    -1, -1, -1,
    -1,  9, -1,
    -1, -1, -1
);

float blurkernel[9] = float[] (
    1.0/16, 2.0/16, 1.0/16,
    2.0/16, 4.0/16, 2.0/16,
    1.0/16, 2.0/16, 1.0/16
);

float edgedetectionkernel[9] = float[] (
    1,  1,  1,
    1, -8,  1,
    1,  1,  1
);

vec3 Reinhard(vec3 color);
vec3 ExposureFactor(vec3 color, float exposure);

void main() {
    vec4 result = vec4(texture(ScreenTexture, texCoords).rgb, 1.0);
    // vec3 color = vec3(0.0, 0.0, 0.0);
    // float kernel[9];

    // switch(KernelIndex) {
    //     case 0:
    //         kernel = noeffectkernel;
    //     break;

    //     case 1:
    //         kernel = sharpenkernel;
    //     break;

    //     case 2:
    //         kernel = blurkernel;
    //     break;

    //     case 3:
    //         kernel = edgedetectionkernel;
    //     break;

    //     default:
    //         kernel = noeffectkernel;
    // }

    // for(int i = 0; i < 9; ++i)
    //     color += kernel[i] * vec3(texture(ScreenTexture, fs_in.texCoords + offsets[i]));

    // vec4 result = vec4(ExposureFactor(color, exposure), 1.0);

    // if(Inversion)
    //     result = inversion(result);

    // if(Grayscale)
    //     result = grayscale(result);

    // // Gamma
    // result.rgb = GammaCorrection ? pow(result.rgb, vec3(1.0 / Gamma)) : result.rgb;
    
    FragColor = result;
}

vec3 Reinhard(vec3 color) {
    return color / (color + vec3(1.0));
}

vec3 ExposureFactor(vec3 color, float exposure) {
    return vec3(1.0) - exp(-color * exposure);
}
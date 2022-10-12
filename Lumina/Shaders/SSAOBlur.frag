#version 330 core

const int SSAO_NOISE_SIZE = 4;

in VS_OUT {
    vec2 texCoords;
} fs_in;

out float FragColor;

uniform sampler2D raw_SSAO;

void main(){
    vec2 tex_size = 1.0 / vec2(textureSize(raw_SSAO, 0));
    int bias = SSAO_NOISE_SIZE / 2;
    float result = 0.0;

    for (int x = -bias; x < bias; ++x) {
        for (int y = -bias; y < bias; ++y) {
            vec2 offset = vec2(float(x), float(y)) * tex_size;
            result += texture(raw_SSAO, fs_in.texCoords + offset).r;
        }
    }

    FragColor = result / (SSAO_NOISE_SIZE * SSAO_NOISE_SIZE);
}
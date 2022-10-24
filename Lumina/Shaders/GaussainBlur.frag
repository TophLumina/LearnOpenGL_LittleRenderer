#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
} fs_in;

uniform sampler2D image;

uniform bool horizontal;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); // Kernal for Gaussain_Blur <Optional>

void main() {
    vec2 pixeloffset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, fs_in.texCoords).rgb * weight[0];

    if (horizontal)
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, fs_in.texCoords + vec2(pixeloffset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, fs_in.texCoords - vec2(pixeloffset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for (int i = 1; i < 5; ++i)
        {
            result += texture(image, fs_in.texCoords + vec2(0.0, pixeloffset.y * i)).rgb * weight[i];
            result += texture(image, fs_in.texCoords - vec2(0.0, pixeloffset.y * i)).rgb * weight[i];
        }
    }

    FragColor = vec4(result, 1.0);
}
# version 330 core

const float Gamma = 2.2;

out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
} fs_in;

uniform sampler2D ScreenTexture;

void main() {
    FragColor = texture(ScreenTexture, fs_in.texCoords);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / Gamma));
}
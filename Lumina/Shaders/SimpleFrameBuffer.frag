# version 330 core

out vec4 FragColor;

in VS_OUT {
    vec2 texCoords;
} fs_in;

uniform sampler2D ScreenTexture;

void main() {
    vec3 col = texture(ScreenTexture, texCoords);
    FragColor = vec4(col, 1.0);
}
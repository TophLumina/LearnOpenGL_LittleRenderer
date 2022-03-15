# version 330 core

out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D ScreenTexture;
uniform bool Grayscale;

void main() {
    FragColor = texture(ScreenTexture, texCoords);
    if(Grayscale) {
        float average = (FragColor.r + FragColor.g + FragColor.b) / 3;
        FragColor = vec4(average, average, average, 1.0);
    }
}
# version 330 core
out vec4 FragColor;

in vec3 Col;
in vec2 Texcoord;

uniform sampler2D aTexture;

void main() {
	FragColor = texture(aTexture, Texcoord) * vec4(Col, 1.0);
}
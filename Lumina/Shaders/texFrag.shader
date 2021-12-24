# version 330 core
out vec4 FragColor;

in vec3 Col;
in vec2 Texcoord;

uniform sampler2D Texture1;
uniform sampler2D Texture2;

void main() {
	FragColor = mix(texture(Texture1, Texcoord), texture(Texture2, vec2(-Texcoord.x, Texcoord.y)), 1);
}
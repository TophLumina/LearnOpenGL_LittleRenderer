# version 330 core

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 lightPos;
uniform float ambientFactor;
uniform float specularFactor;

out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;

void main() {
	//ambient
	vec3 ambient = ambientFactor * lightColor;

	//diffuse
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(lightDir, norm), 0.0);
	vec3 diffuse = diff * lightColor;

	//specular
	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(reflectDir, viewDir), 0.0), 64);
	vec3 specular = specularFactor * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}
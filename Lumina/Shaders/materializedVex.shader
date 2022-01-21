# version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;

void main() {
	mat4 MVP = projection * view * model;

	normal = mat3(transpose(inverse(model * view))) * aNormal;
	fragPos = vec3(view * model * vec4(aPos, 1.0));
	gl_Position = MVP * vec4(aPos, 1.0);
}
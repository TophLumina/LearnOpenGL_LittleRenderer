# version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;

void main() {
	//normal need to be in world space and get rid of scaling matrix
	//and rather than using world space coords it is better to use view space for the camera is fix on the origin(0,0,0)
	normal = mat3(transpose(inverse(view * model))) * aNormal;
	fragPos = vec3(view * model * vec4(aPos, 1.0));
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}
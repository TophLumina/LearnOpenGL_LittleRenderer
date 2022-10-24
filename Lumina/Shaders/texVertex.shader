#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aCol;
layout(location = 2) in vec2 aTexcoord;

out vec3 Col;
out vec2 Texcoord;

void main() {
	gl_Position = vec4(aPos,1.0);
	Col = aCol;
	Texcoord = aTexcoord;
}
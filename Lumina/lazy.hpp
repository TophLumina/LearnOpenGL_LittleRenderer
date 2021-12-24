#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class lazy {
public:
	static void glfwCoreEnv(int min_version, int max_version);
	static unsigned int ShaderBulider(unsigned vertexShader, const GLchar* vertexshadersource, unsigned int fragShader, const GLchar* fragshadersource);
};

void lazy::glfwCoreEnv(int min_version, int max_version) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, min_version);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, max_version);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

unsigned int lazy::ShaderBulider(unsigned vertexShader, const GLchar* vertexshadersource, unsigned int fragShader, const GLchar* fragshadersource) {
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexshadersource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragshadersource, NULL);
	glCompileShader(fragShader);

	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int program;
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	return program;
}
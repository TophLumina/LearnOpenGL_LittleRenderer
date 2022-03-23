#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
public:
	unsigned int ID;
	Shader(const GLchar* vertexpath, const GLchar* fragmentpath) {
		//Reading Shaders from File
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::string vertexCode;
		std::string fragmentCode;
		try {
			vShaderFile.open(vertexpath);
			fShaderFile.open(fragmentpath);

			std::stringstream vShaderStream;
			std::stringstream fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_READ_FAILED" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//Compile Shaders
		unsigned int vertex;
		unsigned int fragment;
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		int success;
		char infoLog[512];

		//check complie errors
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		//Program
		unsigned int program;
		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);

		this->ID = program;
	}

	// Adding Geometry Shader Usage
	Shader(const GLchar* vertexpath, const GLchar* geometrypath ,const GLchar* fragmentpath) {
		//Reading Shaders from File
		std::ifstream vShaderFile;
		std::ifstream gShaderFile;
		std::ifstream fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::string vertexCode;
		std::string geometryCode;
		std::string fragmentCode;
		try {
			vShaderFile.open(vertexpath);
			gShaderFile.open(geometrypath);
			fShaderFile.open(fragmentpath);

			std::stringstream vShaderStream;
			std::stringstream gShaderStream;
			std::stringstream fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			gShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			vShaderFile.close();
			gShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			geometryCode = gShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::SHADER::FILE_READ_FAILED" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char *gShaderCode = geometryCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		//Compile Shaders
		unsigned int vertex;
		unsigned int geometry;
		unsigned int fragment;

		int success;
		char infoLog[512];

		// VERTEX
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		// check complie errors
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// GEOMETRY
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);

		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// FRAGMENT
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}

		// Program
		unsigned int program;
		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, geometry);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			std::cout << "ERROR::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
		}

		glDeleteShader(vertex);
		glDeleteShader(geometry);
		glDeleteShader(fragment);

		this->ID = program;
	};

	void Use() const {
		glUseProgram(this->ID);
	}

	void setBool(std::string name, bool value) const {
		glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value);
	}

	void setInt(std::string name, int value) const {
		glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value);
	}
	void setFloat(std::string name, float value) const {
		glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value);
	}

	void setVec3(std::string name, glm::vec3 value) const {
		glUniform3fv(glGetUniformLocation(this->ID, name.c_str()), 1, glm::value_ptr(value));
	}

	void setMat4(std::string name, glm::mat4 value) const {
		glUniformMatrix4fv(glGetUniformLocation(this->ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
};
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "twoTriangle.hpp"
#include "SimpleShader1.hpp"

//Every time when we change the size of the window this func will be call.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

//Check ESC to close the window
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main() {
	//Window Init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Fail to Create GLFW Windwow" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Triengle Init

	//Create a VertexShader
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//Plug the shader-code to the current Shader and complie it
	glShaderSource(vertexShader, 1, &VertexShaderSource, NULL);
	glCompileShader(vertexShader);


	//Debug log
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SAHDER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}


	//Create a FragShader
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &FragShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Debug log
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SAHDER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	//Link Shaders together to Create a Shader Peogram
	unsigned int shaderPorgram;
	shaderPorgram = glCreateProgram();
	glAttachShader(shaderPorgram, vertexShader);
	glAttachShader(shaderPorgram, fragmentShader);
	glLinkProgram(shaderPorgram);//Done.


	//Debug log
	glGetProgramiv(shaderPorgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderPorgram, 512, NULL, infoLog);
		std::cout << "ERROR::PROGRAM::LINK::FAILED\n" << infoLog << std::endl;
	}


	//After Link the Shader to the Program Delete the Shader
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	//using VAO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	//Binding the parts to VAO-VBO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Tell GPU how to use the Data in Buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertces), vertces, GL_STATIC_DRAW);

	//Tell VAO the Attriibs and enable it
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertces), vertces, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//Rander Loop
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		//Rander Tasks
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Choose the buffers and shaders we use to rander
		glBindVertexArray(VAO);
		glUseProgram(shaderPorgram);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	//End the window
	glfwTerminate();
}
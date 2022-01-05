#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Camera.hpp"
#include "Shader.hpp"
#include "lazy.hpp"
#include "Cube.hpp"

glm::vec3 campos(0.0, 0.0, 3.0);
glm::vec3 up(0.0, 1.0, 0.0);
Camera camera(campos, up);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	lazy::glfwCoreEnv(3, 3);
	int ScreenWidth = 800;
	int ScreenHeight = 600;

	GLFWwindow* window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Color", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD" << std::endl;
		return -1;
	}
	glViewport(0, 0, 800, 600);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	unsigned int VAO;
	unsigned int VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader objectShader("./Shaders/7Vex.shader", "./Shaders/7Fag.shader");
	Shader lightShader("./Shaders/7Vex.shader", "./Shaders/lightingFag.shader");

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(camera.Fov, (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

	glm::vec3 lightpos(1.2f, 1.0f, 2.0f);

	glm::mat4 lightmodel(1.0f);
	lightmodel = glm::translate(lightmodel, lightpos);

}
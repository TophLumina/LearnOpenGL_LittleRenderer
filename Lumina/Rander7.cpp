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

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static float lastxpos = 400.0f;
	static float lastypos = 300.0f;
	static bool enter = true;
	if(enter) {
		enter = false;
		lastxpos = xpos;
		lastypos = ypos;
	}
	float xoffset = xpos - lastxpos;
	float yoffset = -(ypos - lastypos);;
	lastxpos = xpos;
	lastypos = ypos;
	camera.Mouse(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.MouseScroll(yoffset);
}

void input(GLFWwindow* window) {
	static float lasttime = 0.0f;
	static float deltatime = 0.0f;
	float currenttime = glfwGetTime();
	deltatime = currenttime - lasttime;
	lasttime = currenttime;
	float camspeed = camera.MovementSpeed * deltatime;
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Move(FORWARD, deltatime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Move(BACKWARD, deltatime);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Move(RIGHT, deltatime);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Move(LEFT, deltatime);
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glEnable(GL_DEPTH_TEST);

	unsigned int objectVAO;
	unsigned int VBO;
	glGenVertexArrays(1, &objectVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(objectVAO);
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

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Shader objectShader("./Shaders/7Vex.shader", "./Shaders/7Fag.shader");
	Shader lightShader("./Shaders/7Vex.shader", "./Shaders/lightFag.shader");

	glm::vec3 lightpos(1.2f, 1.0f, 2.0f);

	glm::mat4 lightmodel(1.0f);
	lightmodel = glm::translate(lightmodel, lightpos);
	lightmodel = glm::scale(lightmodel, glm::vec3(0.2f));

	glm::vec3 lightcolor(1.0f, 1.0f, 1.0f);
	glm::vec3 objectcolor(1.0f, 0.5f, 0.31f);

	//the model matrix and colors will not change during the rander loop
	lightShader.Use();
	lightShader.setVec3("lightColor", lightcolor);
	lightShader.setMat4("medel", lightmodel);

	objectShader.Use();
	objectShader.setVec3("lightColor", lightcolor);
	objectShader.setVec3("objectColor", objectcolor);
	objectShader.setMat4("model", glm::mat4(1.0f));

	while(!glfwWindowShouldClose(window)) {
		input(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Fov, (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

		glBindVertexArray(objectVAO);

		objectShader.Use();
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//light do not function well
		//need debug
		glBindVertexArray(lightVAO);

		lightShader.Use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &lightVAO);
	glDeleteBuffers(1, &objectVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
}
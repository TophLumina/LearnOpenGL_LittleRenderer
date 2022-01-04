#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "lazy.hpp"
#include "Cube.hpp"
#include "Camera.hpp"

//access through input
glm::vec3 campos(0.0f, 0.0f, 3.0f);
glm::vec3 camup(0.0f, 1.0f, 0.0f);

Camera camera(campos, camup);

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	//traking time cost eachframe
	static float lasttime = 0.0f;
	static float deltatime = 0.0f;

	float currenttime = glfwGetTime();
	deltatime = currenttime - lasttime;
	lasttime = currenttime;
	float camspeed = 2.5f * deltatime;

	//FPS like camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.KeyBoard(FORWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.KeyBoard(BACKWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.KeyBoard(RIGHT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.KeyBoard(LEFT, deltatime);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

//mouse callback func
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	//center of the screen space
	static float lastx = 400.0f;
	static float lasty = 300.0f;

	//offsets
	float xoffset = xpos - lastx;
	float yoffset = -(ypos - lasty);//be ware

	lastx = xpos;
	lasty = ypos;

	camera.Mouse(xoffset, yoffset);
}

//zoom func
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.MouseScroll(yoffset);
}

int main() {
	lazy::glfwCoreEnv(3, 3);

	int Screen_Width = 800;
	int Screen_Height = 600;

	GLFWwindow* window = glfwCreateWindow(Screen_Width, Screen_Height, "Cubes", NULL, NULL);
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

	glViewport(0, 0, Screen_Width, Screen_Height);

	//mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	//scroll input
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	unsigned int VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Shader aShader("./Shaders/cubeMVP.shader", "./Shaders/cubeFrag.shader");

	int width1 = 800;
	int height1 = 800;
	int colChannel1 = 24;

	int width2 = 512;
	int height2 = 512;
	int colChannel2 = 32;

	const char* loaderror = "Failed to load texture";

	stbi_set_flip_vertically_on_load(true);

	unsigned char* textureData1 = stbi_load("./Texture/Arknights.png", &width1, &height1, &colChannel1, 0);
	if (!textureData1)
		std::cout << loaderror << std::endl;

	unsigned char* textureData2 = stbi_load("./Texture/awesomeface.png", &width2, &height2, &colChannel2, 0);
	if (!textureData2)
		std::cout << loaderror << std::endl;

	//Texture1:
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData1);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(textureData1);

	//Texture2:
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData2);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(textureData2);

	glBindTexture(GL_TEXTURE_2D, 0);

	aShader.Use();
	aShader.setInt("Texture1", 0);
	aShader.setInt("Texture2", 1);

	//enable z_buffer test
	glEnable(GL_DEPTH_TEST);

	//poses of the cubes
	glm::vec3 cubePositions[] = {
  glm::vec3(0.0f,  0.0f,  0.0f),
  glm::vec3(2.0f,  5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3(2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f,  3.0f, -7.5f),
  glm::vec3(1.3f, -2.0f, -2.5f),
  glm::vec3(1.5f,  2.0f, -2.5f),
  glm::vec3(1.5f,  0.2f, -1.5f),
  glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	float par = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

		glBindVertexArray(VAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		aShader.Use();

		glm::mat4 view(1.0f);
		view = camera.GetViewMatrix();

		glm::mat4 projection(1.0f);
		projection = glm::perspective(glm::radians(camera.Fov), (float)Screen_Width/(float)Screen_Height, 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(aShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(aShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		//pass the matrix to the fragshader

		for (glm::vec3 v : cubePositions) {
			glm::mat4 model(1.0f);
			model = glm::translate(model, v);
			model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

			glUniformMatrix4fv(glGetUniformLocation(aShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

			aShader.setFloat("par", par);
			if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
				par = par + 0.0002f > 1.0f ? 1.0f : par + 0.0002f;
			if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
				par = par - 0.0002f < 0.0f ? 0.0f : par - 0.0002f;

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glDeleteBuffers(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
}
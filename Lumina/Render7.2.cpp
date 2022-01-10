#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Camera.hpp"
#include "Shader.hpp"
#include "lazy.hpp"
#include "CubewithNormal.hpp"

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
	if (enter) {
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
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Move(FORWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Move(BACKWARD, deltatime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Move(RIGHT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Move(LEFT, deltatime);
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
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

	const char* glsl_version = "#version 330 core";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui::StyleColorsLight();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	bool main_window = true;

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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Shader objectShader("./Shaders/materialVex.shader", "./Shaders/materialFag.shader");
	Shader lightShader("./Shaders/7Vex.shader", "./Shaders/lightFag.shader");

	glm::vec3 lightpos(1.2f, 1.0f, 2.0f);

	glm::mat4 lightmodel(1.0f);
	lightmodel = glm::translate(lightmodel, lightpos);
	lightmodel = glm::scale(lightmodel, glm::vec3(0.2f));

	//light and material
	glm::vec3 light_source(1.0f);
	glm::vec3 light_ambient(0.2f);
	glm::vec3 light_diffuse(0.5f);
	glm::vec3 light_specular(1.0f);

	glm::vec3 material_ambient(1.0f, 0.5f, 0.31f);
	glm::vec3 material_diffuse(1.0f, 0.5f, 0.31f);
	glm::vec3 material_specular(0.5f);
	float material_shininess = 32.0f;

	//the model matrix will not change during the render loop
	lightShader.Use();
	lightShader.setMat4("model", lightmodel);
	lightShader.setVec3("light.ambient", light_source);	//here use the original color which is (1.0, 1.0, 1.0)

	objectShader.Use();
	objectShader.setMat4("model", glm::mat4(1.0f));
	objectShader.setVec3("light.ambient", light_ambient);
	objectShader.setVec3("light.diffuse", light_diffuse);
	objectShader.setVec3("light.specular", light_specular);

	objectShader.setVec3("material.ambient", material_ambient);
	objectShader.setVec3("material.diffuse", material_diffuse);
	objectShader.setVec3("material.specular", material_specular);
	objectShader.setFloat("material.shininess", material_shininess);

	while (!glfwWindowShouldClose(window)) {
		input(window);
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (main_window) {
			// Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui
			// ImGui::ShowDemoWindow();
			ImGui::Begin("Debug Page", &main_window);

			ImGui::Text("Press TAB to switch Cursor capture mode.");

			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

			ImGui::End();
		}

		ImGui::Render();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(camera.Fov, (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

		objectShader.Use();

		//pass lightPos to the frag shader
		//and if we use view space coords the light pos must be updated while we moving the camera
		//and maybe it is more efficient if we put matrix transforms in cpu (in most cases)
		objectShader.setVec3("light.position", glm::mat3(view) * lightpos);
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);

		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightShader.Use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteBuffers(1, &lightVAO);
	glDeleteBuffers(1, &objectVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
}

// TODO:
// - findout why the lighting looks strange in range
// - compelete the new shader and put it into use in Render7.2
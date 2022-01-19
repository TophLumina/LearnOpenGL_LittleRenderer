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
		if(glfwGetInputMode(window,GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
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

	bool deemo_window = true;
	bool another_window = false;

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

	//pointer to vertex pos and vertex normal
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	Shader objectShader("./Shaders/PhongVex.shader", "./Shaders/PhongFag.shader");
	Shader lightShader("./Shaders/7Vex.shader", "./Shaders/lightFag.shader");

	glm::vec3 lightpos(1.2f, 1.0f, 2.0f);

	glm::mat4 lightmodel(1.0f);
	lightmodel = glm::translate(lightmodel, lightpos);
	lightmodel = glm::scale(lightmodel, glm::vec3(0.2f));

	glm::vec3 lightcolor(1.0f, 1.0f, 1.0f);
	glm::vec3 objectcolor(1.0f, 0.5f, 0.31f);

	//the model matrix and colors will not change during the render loop
	lightShader.Use();
	lightShader.setVec3("lightColor", lightcolor);
	lightShader.setMat4("model", lightmodel);

	objectShader.Use();
	objectShader.setVec3("lightColor", lightcolor);
	objectShader.setVec3("objectColor", objectcolor);
	objectShader.setMat4("model", glm::mat4(1.0f));

	float ambient = 0.1f;
	float specular = 0.4f;

	while (!glfwWindowShouldClose(window)) {
		input(window);
		glfwPollEvents();

		//start the imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//a imgui deemo window
		if (deemo_window) {
			// Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui
			// ImGui::ShowDemoWindow();
			ImGui::Begin("Debug Page");

			//Pass the pointer of the var to link the checkbox and var
			ImGui::Text("Page Selector");
			ImGui::Checkbox("Main Page", &deemo_window);
			ImGui::Checkbox("Another Page", &another_window);

			ImGui::Text("Press TAB to switch Cursor capture mode.");

			ImGui::SliderFloat("Ambient", &ambient, 0.0f, 1.0f);
			ImGui::SliderFloat("Speclar", &specular, 0.0f, 1.0f);

			ImGui::Text("LightPos: %.1f,%.1f,%.1f", lightpos.x, lightpos.y, lightpos.z);

			ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

			ImGui::End();
		}

		if (another_window) {
			//A window begins with a Begin() and ends with a End()
			ImGui::Begin("Another Window", &another_window);
			// Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

			ImGui::Text("I haven't descide what to put on yet.");
			//Text use to show any kind of string text;

			if (ImGui::Button("Close"))
				another_window = false;
			//Button use to create a button

			ImGui::End();
		}

		//all element need to be render after crafted
		ImGui::Render();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

		objectShader.Use();
		//pass the ambientFactor and the speclarFactor to the shader
		objectShader.setFloat("ambientFactor", ambient);
		objectShader.setFloat("specularFactor", specular);

		//pass lightPos to the frag shader
		//and if we use view space coords the light pos must be updated while we moving the camera
		//and maybe it is more efficient if we put matrix transforms in cpu (in most cases)
		objectShader.setMat4("view", view);
		objectShader.setMat4("projection", projection);
		objectShader.setVec3("lightPos", glm::vec3(view * glm::vec4(lightpos, 1.0)));

		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightShader.Use();
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);

		glBindVertexArray(objectVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//Draw(I don't know how it works out)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
	//clean up
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteBuffers(1, &lightVAO);
	glDeleteBuffers(1, &objectVAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
}
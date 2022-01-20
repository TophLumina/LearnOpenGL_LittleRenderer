#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "lazy.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "CubewithNormal.hpp"

glm::vec3 campos(0.0, 0.0, 3.0);
glm::vec3 up(0.0, 1.0, 0.0);
Camera camera(campos, up);

bool enter = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastxpos = 400.0f;
    static float lastypos = 300.0f;
    if(enter) {
        enter = false;
        lastxpos = xpos;
        lastypos = ypos;
    }
    float xoffset = xpos - lastxpos;
    float yoffset = -(ypos - lastypos);
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
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPosCallback(window, NULL);
		enter = true;
	}
	else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, mouse_callback);
	}
}

int main() {
    lazy::glfwCoreEnv(3, 3);
    int ScreenWidth = 800;
    int ScreenHeight = 600;

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Material", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }

    const char *glsl_version = "#version 330 core";

    //imGUI settings
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool debug_page = true;

    glViewport(0, 0, ScreenWidth, ScreenHeight);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);

    unsigned int VBO;
    unsigned int objectVAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &objectVAO);

    glBindVertexArray(objectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

    Shader lightShader("./Shaders/7Vex.shader", "./Shaders/lightFag.shader");
    Shader objectShader("./Shaders/materializedVex.shader", "./Shaders/materializedFag.shader");

    glm::vec3 lightpos(1.2f, 1.0f, 2.0f);
    glm::mat4 lightmodel(1.0f);
    lightmodel = glm::translate(lightmodel, lightpos);
    lightmodel = glm::scale(lightmodel, glm::vec3(0.2f));

    glm::vec3 lightcolor(1.0f, 1.0f, 1.0f);
    glm::vec3 objectsurfacecolor(1.0f, 0.5f, 0.31f);

    lightShader.Use();
    lightShader.setVec3("lightColor", lightcolor);
    lightShader.setMat4("model", lightmodel);

    // and don't forget that the address of the shader was exposed only after the shader is used --<important>--
    objectShader.Use();
    objectShader.setMat4("model", glm::mat4(1.0f));
    objectShader.setVec3("material.ambient", objectsurfacecolor);
    objectShader.setVec3("material.diffuse", objectsurfacecolor);
    objectShader.setVec3("material.specular", 0.5f * lightcolor);
    objectShader.setFloat("material.shininess", 32.0f);

    objectShader.setVec3("light.ambient", 0.1f * lightcolor);
    objectShader.setVec3("light.diffuse", 0.5f * lightcolor);
    objectShader.setVec3("light.specular", 1.0f * lightcolor);

    while (!glfwWindowShouldClose(window)) {
        input(window);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(debug_page) {
            ImGui::Begin("Debug Page");
            //stuff use to dynamicly debug
            ImGui::Text("camera:(%.1f, %.1f, %.1f)", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::Text("FPS:%.1f", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

        lightShader.Use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        objectShader.Use();
        objectShader.setMat4("view",view);
        objectShader.setMat4("projection",projection);
        //the lightpos should be in view coords for we caculate lighting in the view space
        objectShader.setVec3("light.position", glm::vec3(view * glm::vec4(lightpos, 1.0)));

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

//todo: compelete the UI and make the material adjustable in real-time
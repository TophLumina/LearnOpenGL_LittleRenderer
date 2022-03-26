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

#include "Light.hpp"

// #define DEBUG_TEST

#include "./Shaders/Model.hpp"

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

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Model Loading", NULL, NULL);
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

    Shader modelshader("./Shaders/ModelwithLighting.vert", "./Shaders/ModelwithLighting.frag");

    Model Haku("./Model/Haku/TDA Lacy Haku.pmx");

    modelshader.Use();

    glm::mat4 model(1.0f);
    modelshader.setMat4("model", model);

    //Lights
    Light light(&modelshader);
    glm::vec3 light_color(1.0f, 1.0f, 1.0f);

    light.num_Dirlight += 1;

    float Dir[3] = {-1.0f, -1.0f, -1.0f};

    while (!glfwWindowShouldClose(window)) {
        input(window);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(debug_page) {
            ImGui::Begin("Debug Page");
            //stuff use to dynamicly debug

            ImGui::BulletText("Camera Pos:(%.1f, %.1f, %.1f)", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::BulletText("Current Time: %.1fs", (float)glfwGetTime());
            ImGui::BulletText("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::NewLine();

            ImGui::SliderFloat3("DirLight", Dir, -1.0f, 1.0f);

            ImGui::End();
        }

        ImGui::Render();

        //Dynamic CTRL
        glm::vec3 Dirlight_dir(Dir[0], Dir[1], Dir[2]);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

        modelshader.Use();

        modelshader.setMat4("view", view);
        modelshader.setMat4("projection", projection);

        //Lights Config
        // light.updateDirlight(0, glm::vec3(view * glm::vec4(Dirlight_dir, 1.0f)), 0.1f * light_color, 0.5f * light_color, 1.0f * light_color);

        //BLight Config
        light.updateDirlight(0, glm::vec3(view * glm::vec4(Dirlight_dir, 1.0f)), 0.6f * light_color, 0.4f * light_color, 1.0f * light_color);

        Haku.Draw(&modelshader);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
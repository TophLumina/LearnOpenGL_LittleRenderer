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
#include "Camera.hpp"
#include "Shader.hpp"
#include "./Shaders/Model.hpp"

glm::vec3 campos(0.0f, 0.0f, 0.0f);
glm::vec3 camup(0.0f, 1.0f, 0.0f);

Camera camera(campos, camup);

bool enter = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static float lastxpos = 400;
    static float lastypos = 300;
    if(enter) {
        lastxpos = xpos;
        lastypos = ypos;
        enter=false;
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
    if(glfwGetKey(window,GLFW_KEY_W) == GLFW_PRESS)
        camera.Move(FORWARD, deltatime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.Move(BACKWARD, deltatime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Move(RIGHT, deltatime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.Move(LEFT, deltatime);
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
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

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Aphla Blending", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }

    const char *glsl_version = "#version 330 core";

    // imGUI init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsLight();

    // Backends init
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Status
    bool main_page = true;

    // Callback funcs
    glViewport(0, 0, ScreenWidth, ScreenHeight);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Modify Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Blening Enable
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Blending Factors
    // TODO::Need to use more advanced AphlaBlending Mathematics

    // Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Decide what faces should be culled
    glFrontFace(GL_CCW); // Decide what faces should be defined as front face -- CCW -- VertexOrder::CounterClockWise (in View Space)

    Shader modelshader("./Shaders/ExternalModel.vert", "./Shaders/AphlaBlending.frag");

    // Model need for testing
    Model test_model("./Model/Haku/TDA Lacy Haku.pmx");

    modelshader.Use();
    glm::mat4 model(1.0f);
    // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelshader.setMat4("model", model);

    while(!glfwWindowShouldClose(window)) {
        input(window);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(main_page) {
            ImGui::Begin("Status");

            // Status
            ImGui::BulletText("Time: %.1f", (float)glfwGetTime());
            ImGui::BulletText("FPS: %.1f", ImGui::GetIO().Framerate);

            ImGui::End();
        }

        ImGui::Render();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Render Code Here
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

        modelshader.Use();

        modelshader.setMat4("view", view);
        modelshader.setMat4("projection", projection);

        modelshader.Use();
        test_model.Draw(modelshader);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
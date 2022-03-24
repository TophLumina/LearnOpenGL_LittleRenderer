#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "lazy.hpp"
// #include "Camera.hpp"
#include "Shader.hpp"
#include "Screen.hpp"

#include "./Shaders/FrameBuffer.hpp"

// glm::vec3 campos(0.0, 0.0, 0.0);
// glm::vec3 camup(0.0, 1.0, 0.0);

// Camera camera(campos, camup);

// Geometry Shader input
float points[] = {
    // Pos           // Col
    -0.5f, 0.5f,    1.0f, 0.0f, 0.0f,
    0.5f, 0.5f,     0.0f, 1.0f, 0.0f,
    0.5f, -0.5f,    0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,   1.0f, 1.0f, 0.0f};

// bool enter = true;

// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }

// void mouse_callback(GLFWwindow* window, double xpos, double ypos)
// {
//     static float lastxpos = 400;
//     static float lastypos = 300;
//     if(enter)
//     {
//         lastxpos = xpos;
//         lastypos = ypos;
//         enter = false;
//     }
//     float xoffset = xpos - lastxpos;
//     float yoffset = ypos - lastypos;
//     lastxpos = xpos;
//     lastypos = ypos;
//     camera.Mouse(xoffset, yoffset);
// }

// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
// {
//     camera.MouseScroll(yoffset);
// }

// void inputs(GLFWwindow* window)
// {
//     static float lastframe_time = 0;
//     static float deltatime = 0;
//     float currentframe_time = glfwGetTime();
//     deltatime = currentframe_time - lastframe_time;
//     lastframe_time = currentframe_time;

//     if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//         camera.Move(FORWARD, deltatime);
//     if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//         camera.Move(LEFT, deltatime);
//     if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//         camera.Move(BACKWARD, deltatime);
//     if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//         camera.Move(RIGHT, deltatime);

//     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);

//     if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
//     {
//         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//         glfwSetCursorPosCallback(window, NULL);
//         enter = true;
//     }
//     else
//     {
//         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//         glfwSetCursorPosCallback(window, mouse_callback);
//     }
// }

int main()
{
    lazy::glfwCoreEnv(3, 3);
    int ScreenWidth = 800;
    int ScreenHeight = 600;

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Geometry Shader", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }

    const char *glsl_version = "#version 330 core";

    // imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsLight();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool main_page = true;

    // Stuffs todo Here
    Shader GeoTestshader("./Shaders/Geo_test.vert", "./Shaders/Geo_test.geom", "./Shaders/Geo_test.frag");
    unsigned int pointsVAO;
    glGenVertexArrays(1, &pointsVAO);
    unsigned int pointsVBO;
    glGenBuffers(1, &pointsVBO);

    glBindVertexArray(pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glPointSize(10);

    FrameBuffer fbo(ScreenWidth, ScreenHeight);

    // Shader for Screen
    Shader screenshader("./Shaders/OffScreen.vert", "./Shaders/SimpleFrameBuffer.frag");

    while(!glfwWindowShouldClose(window))
    {
        // inputs(window);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(main_page)
        {
            ImGui::Begin("Status");

            ImGui::End();
        }

        ImGui::Render();

        glBindFramebuffer(GL_FRAMEBUFFER, fbo.ID);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        GeoTestshader.Use();
        glBindVertexArray(pointsVAO);
        glDrawArrays(GL_POINTS, 0, 4);
        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        screenshader.Use();
        glBindTexture(GL_TEXTURE_2D, fbo.texture_attachment);
        glBindVertexArray(fbo.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    fbo.Delete();
    glDeleteBuffers(1, &pointsVBO);
    glDeleteBuffers(1, &pointsVAO);

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
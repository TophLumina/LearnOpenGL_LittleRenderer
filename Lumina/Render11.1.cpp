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
#include "Camera.hpp"
#include "Shader.hpp"
#include "Screen.hpp"
#include "Model.hpp"

glm::vec3 campos(0.0, 0.0, 0.0);
glm::vec3 camup(0.0, 1.0, 0.0);

Camera camera(campos, camup);

bool enter = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static float lastxpos = 400;
    static float lastypos = 300;
    if(enter)
    {
        lastxpos = xpos;
        lastypos = ypos;
        enter = false;
    }
    float xoffset = xpos - lastxpos;
    float yoffset = ypos - lastypos;
    lastxpos = xpos;
    lastypos = ypos;
    camera.Mouse(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.MouseScroll(yoffset);
}

void inputs(GLFWwindow* window)
{
    static float lastframe_time = 0;
    static float deltatime = 0;
    float currentframe_time = glfwGetTime();
    deltatime = currentframe_time - lastframe_time;
    lastframe_time = currentframe_time;

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.Move(FORWARD, deltatime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.Move(LEFT, deltatime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.Move(BACKWARD, deltatime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.Move(RIGHT, deltatime);

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, NULL);
        enter = true;
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
    }
}

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

    // FrameBuffer Usage
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Texture_Attachment Settings
    unsigned int texture_attachment;
    glGenTextures(1, &texture_attachment);
    glBindTexture(GL_TEXTURE_2D, texture_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Attach it to FrameBuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_attachment, 0);

    // RenderBuffers are usually Write_ONLY, so it mostly use for Storeing Depth and Stencil. we need Depth and Stencil for Depth_test and Stencil_test but hardly sampling them
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach it to FrameBuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Check the Bound Framebuffer
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: FrameBuffer is NOT Compelete." << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Load VAO and VBO for Screen
    unsigned int ScreenVAO;
    glGenVertexArrays(1, &ScreenVAO);
    glBindVertexArray(ScreenVAO);

    unsigned int ScreenVBO;
    glGenBuffers(1, &ScreenVBO);
    glBindBuffer(GL_ARRAY_BUFFER, ScreenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertces), &vertces, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // fin

    // Shader for Screen
    Shader screenshader("./Shaders/OffScreen.vert", "./Shaders/SimpleFrameBuffer.frag");

    // Model with Shader
    Model Haku("./Model/Haku/TDA Lacy Haku.pmx");
    Shader HakuShader("./Shaders/Explode.vert", "./Shaders/Explode.geom", "./Shaders/Explode.frag");

    // UniformBuffer Init
    unsigned int uboMatricesBlock;
    glGenBuffers(1, &uboMatricesBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatricesBlock);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

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

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Render Code Here


        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        screenshader.Use();
        glBindVertexArray(ScreenVAO);
        glBindTexture(GL_TEXTURE_2D, texture_attachment);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    glDeleteFramebuffers(1, &fbo);
    glDeleteBuffers(1, &ScreenVBO);
    glDeleteBuffers(1, &ScreenVAO);

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
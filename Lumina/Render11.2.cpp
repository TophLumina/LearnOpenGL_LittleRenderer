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
#include "./Shaders/Model.hpp"
#include "./Shaders/FrameBuffer.hpp"

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
    float yoffset = -(ypos - lastypos);
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

    // CallBacks
    glViewport(0, 0, ScreenWidth, ScreenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FrameBuffer fbo(ScreenWidth, ScreenHeight);

    // Shader for Screen
    Shader screenshader("./Shaders/OffScreen.vert", "./Shaders/SimpleFrameBuffer.frag");

    // Model and Shader
    Model Haku("./Model/Haku/TDA Lacy Haku.pmx");
    Shader HakuShader("./Shaders/EMInterface_Block.vert", "./Shaders/EMInterface_Block.frag");
    // Visualize Normal
    Shader VNShader("./Shaders/VisualizeNormal.vert", "./Shaders/VisualizeNormal.geom", "./Shaders/VisualizeNormal.frag");

    HakuShader.Use();
    glm::mat4 model(1.0f);
    HakuShader.setMat4("model", model);

    VNShader.Use();
    VNShader.setMat4("model", model);

    // UniformBuffer Init
    unsigned int uboMatricesBlock;
    glGenBuffers(1, &uboMatricesBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatricesBlock);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Shader Binding
    HakuShader.Use();
    unsigned int HAKU_Matrices_Index = glGetUniformBlockIndex(HakuShader.ID, "Matrices");
    glUniformBlockBinding(HakuShader.ID, HAKU_Matrices_Index, 0);

    VNShader.Use();
    unsigned int VN_Matrices_Index = glGetUniformBlockIndex(VNShader.ID, "Matrices");
    glUniformBlockBinding(VNShader.ID, VN_Matrices_Index, 0);

    // UniformBlock Binding
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatricesBlock);

    while(!glfwWindowShouldClose(window))
    {
        inputs(window);
        glfwPollEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(main_page)
        {
            ImGui::Begin("Status");

            ImGui::BulletText("Time: %.1fs", (float)glfwGetTime());
            ImGui::BulletText("FPS: %.1f", ImGui::GetIO().Framerate);

            ImGui::End();
        }

        ImGui::Render();

        // Matrices
        glm::mat4 view = camera.GetViewMatrix();

        // Update UniformBuffer
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatricesBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo.ID);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Render Code Here
        HakuShader.Use();
        Haku.Draw(&HakuShader);

        VNShader.Use();
        Haku.Draw(&VNShader);

        glBindVertexArray(0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        screenshader.Use();
        glBindVertexArray(fbo.VAO);
        glBindTexture(GL_TEXTURE_2D, fbo.texture_attachment);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    fbo.Delete();

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
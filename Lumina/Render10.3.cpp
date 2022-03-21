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

#include "Screen.hpp"
#include "./Shaders/SkyBox.hpp"

// FrameBuffer and OffScreenRendering
#define ENABLE_TEXTURE_ATTACHMENT ;
#define ENABLE_RENDERBUFFER_OBJECT ;

// Bulit_in Vars Testing Options
#define GL_FRONTFACING_TEST ;

glm::vec3 campos(0.0f, 0.0f, 0.0f);
glm::vec3 camup(0.0f, 1.0f, 0.0f);

Camera camera(campos, camup);

bool enter = true;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    static float lastxpos = 400;
    static float lastypos = 300;
    if (enter)
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

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.MouseScroll(yoffset);
}

void input(GLFWwindow *window)
{
    static float lasttime = 0.0f;
    static float deltatime = 0.0f;
    float currenttime = glfwGetTime();
    deltatime = currenttime - lasttime;
    lasttime = currenttime;
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
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
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

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Post Effects", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Status
    bool main_page = true;

    // Callback funcs
    glViewport(0, 0, ScreenWidth, ScreenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // Modify Depth Test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Blening Enable
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Blending Factors
    // TODO::Need to use more advanced AphlaBlending Mathematics

    // Face Culling
    // To Test GLSL Bulit_in Var: gl_FrontFacing Face_Culling should be DISABLE
#ifndef GL_FRONTFACING_TEST
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
#endif

    // Shader modelshader("./Shaders/ExternalModel.vert", "./Shaders/AphlaBlending.frag");
    // Shader modelshader("./Shaders/ExternalModel.vert", "./Shaders/EnvironmentMapping.frag");
    Shader modelshader("./Shaders/ExternalModel.vert", "./Shaders/BulidinVarsTest.frag");

    // Model need for testing
    Model test_model("./Model/Haku/TDA Lacy Haku.pmx");

    modelshader.Use();
    glm::mat4 model(1.0f);
    // model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    modelshader.setMat4("model", model);

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Add and Bind Attachments and BUffers
#ifdef ENABLE_TEXTURE_ATTACHMENT
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
#endif

#ifdef ENABLE_RENDERBUFFER_OBJECT
    // RenderBuffers are usually Write_ONLY, so it mostly use for Storeing Depth and Stencil. we need Depth and Stencil for Depth_test and Stencil_test but hardly sampling them
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Attach it to FrameBuffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

#endif

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
    Shader screenshader("./Shaders/OffScreen.vert", "./Shaders/OffScreen.frag");

    // Status
    // Wireframe Mode
    bool modelwireframe = false;
    // Post Effects
    bool inversion = false;
    bool grayscale = false;
    int kernelindex = 0;

    // CubeMap and Skybox
    std::vector<std::string> CubeTexPath{
        "./Texture/SkyBoxTexture/right.jpg",
        "./Texture/SkyBoxTexture/left.jpg",
        "./Texture/SkyBoxTexture/top.jpg",
        "./Texture/SkyBoxTexture/bottom.jpg",
        "./Texture/SkyBoxTexture/front.jpg",
        "./Texture/SkyBoxTexture/back.jpg"
    };

    unsigned int CubeMapTexture = LoadCubeMap(CubeTexPath);

    unsigned int skyboxVAO;
    unsigned int skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Shader skyboxShader("./Shaders/SkyBox.vert", "./Shaders/SkyBox.frag");

    while (!glfwWindowShouldClose(window))
    {
        input(window);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (main_page)
        {
            ImGui::Begin("Status");

            // Status
            ImGui::BulletText("Time: %.1fs", (float)glfwGetTime());
            ImGui::BulletText("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::Checkbox("Wire Frame Mode", &modelwireframe);
            ImGui::BulletText("Effects:");
            ImGui::Checkbox("Inversion", &inversion);
            ImGui::Checkbox("GrayScale", &grayscale);
            ImGui::NewLine();
            ImGui::BulletText("Post Effect Kernels Available:");
            ImGui::BulletText("0::NoEffect\t1::Sharpen\t2::Blur\t3::EdgeDetection");// Only 4 kernels
            ImGui::SliderInt("Kernel Selector", &kernelindex, 0, 3);

            ImGui::End();
        }

        ImGui::Render();

        // First Render Image to the FrameBuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glEnable(GL_DEPTH_TEST);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

        // There is more effecient way to render skybox

        // // Render SkyBox First
        // glDepthMask(GL_FALSE);
        // skyboxShader.Use();
        // skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        // skyboxShader.setMat4("projection", projection);

        // glBindVertexArray(skyboxVAO);
        // // SkyBox Texture
        // glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTexture);
        // glDrawArrays(GL_TRIANGLES, 0, 36);
        // // Enable DepthMasking
        // glDepthMask(GL_TRUE);

        modelshader.Use();
        modelshader.setMat4("view", view);
        modelshader.setMat4("projection", projection);

        // Dynamic Debug
        if(modelwireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

        modelshader.Use();
        glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTexture);
        test_model.Draw(modelshader);

        // Use PRE_DEPTH_TEST to render skybox
        // for the z-coords of the skybox are always 1.0(max) so it can ONLY be seen when nothing has a less z value in the pixel.
        skyboxShader.Use();
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(view)));
        skyboxShader.setMat4("projection",projection);

        glBindVertexArray(skyboxVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Return to the default FrameBuffer and Render the Image on it
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Don't need Depth Test now
        glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(ScreenVAO);
        screenshader.Use();

        // Post Effect Dynamic
        screenshader.setBool("Inversion", inversion);
        screenshader.setBool("Grayscale", grayscale);
        screenshader.setInt("KernelIndex", kernelindex);

        glBindTexture(GL_TEXTURE_2D, texture_attachment);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        // fin

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        }
    glDeleteFramebuffers(1, &fbo); // Delete framebuffer
    glDeleteVertexArrays(1, &ScreenVAO); // Delete BUffers
    glDeleteBuffers(1, &ScreenVBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
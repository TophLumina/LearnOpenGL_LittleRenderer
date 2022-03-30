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

#include "./LightUpdated.hpp"

glm::vec3 campos(0.0, 0.0, 0.0);
glm::vec3 camup(0.0, 1.0, 0.0);

Camera camera(campos, camup);

bool enter = true;

// MultiSampling
int multisample = 4;

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

    // This Func Should be Called before the Window being Created
    glfwWindowHint(GLFW_SAMPLES, multisample);

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Instance Rendering Application", NULL, NULL);
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

    // Imgui
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

    // GL_ENABLES

    // Enable by default
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // FrameBuffer
    FrameBuffer fb(ScreenWidth, ScreenHeight, 8);
    Shader fbShader("./Shaders/OffScreen.vert", "./Shaders/OffScreen.frag");

    // Models and Shaders
    Model Haku("./Model/Haku/TDA Lacy Haku.pmx");
    Shader HakuShader("./Shaders/Blinn_Phong.vert", "./Shaders/Blinn_Phong.frag");

    Model Cube("./Model/JustCube/untitled.fbx");
    Shader CubeShader("./Shaders/instanceCube.vert", "./Shaders/InstanceCube.frag");

    glm::mat4 model(1.0f);
    HakuShader.Use();
    HakuShader.setMat4("model", model);

    // UniformBuffer
    unsigned int MatricesBlock;
    glGenBuffers(1, &MatricesBlock);
    glBindBuffer(GL_UNIFORM_BUFFER, MatricesBlock);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenWidth, 0.1f, 100.0f);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Shader UniformBlock Binding
    HakuShader.Use();
    unsigned int HAKU_Matrices_Index = glGetUniformBlockIndex(HakuShader.ID, "Matrices");
    glUniformBlockBinding(HakuShader.ID, HAKU_Matrices_Index, 0);

    CubeShader.Use();
    unsigned int CUBE_Matrices_Index = glGetUniformBlockIndex(CubeShader.ID, "Matrices");
    glUniformBlockBinding(CubeShader.ID, CUBE_Matrices_Index, 0);

    // UniformbLock Slot Binding
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, MatricesBlock);

    // Ring Positioning Vars
    unsigned int num = 15000;
    glm::mat4 *Instancemodel;
    Instancemodel = new glm::mat4[num];
    srand(static_cast<unsigned int>(glfwGetTime()));
    // Scattering -- Radius of Circle
    float R = 25.0f;
    float offset = 5.0f;
    for (unsigned int i = 0; i < num; ++i)
    {
        glm::mat4 model(1.0f);
        float angle = (float)i / (float)num * 360.0f;
        float alias = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * R + alias;
        alias = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = 8.0f * alias + 20.0f;
        alias = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * R + alias;
        // Location Config

        model = glm::translate(model, glm::vec3(x, y, z));

        // Scale
        float scale = static_cast<float>((rand() % 140) / 100.0f + 0.2f);
        model = glm::scale(model, glm::vec3(scale));

        float rotate = static_cast<float>(rand() % 360);
        model = glm::rotate(model, rotate, glm::vec3(0.4f, 0.6f, 0.8f)); // Rotate Axis

        Instancemodel[i] = model;
    }

    // Colors
    glm::vec3 *colors;
    colors = new glm::vec3[num];
    for (unsigned int i = 0; i < num; ++i)
    {
        float R = static_cast<float>(rand() % 50 + 50);
        float G = static_cast<float>(rand() % 50 + 50);
        float B = static_cast<float>(rand() % 50 + 50);

        glm::vec3 col(R, G, B);
        col /= 100;

        colors[i] = col;
    }

    // Buffer used for Instance Rendering
    unsigned int InstanceMatrices;
    glGenBuffers(1, &InstanceMatrices);
    glBindBuffer(GL_ARRAY_BUFFER, InstanceMatrices);
    glBufferData(GL_ARRAY_BUFFER, num * sizeof(glm::mat4), Instancemodel, GL_STATIC_DRAW);
    std::vector<Mesh> CubeMesh = Cube.ServeMeshes();

    for (unsigned int i = 0; i < CubeMesh.size(); ++i)
    {
        unsigned int currentVAO = CubeMesh[i].ServeVAO();
        glBindVertexArray(currentVAO);

        // OPENGL can receive 4 float-type data in one single layer
        // so devide a mat4 into 4 vec4 and sent them in 4 different layers
        GLsizei sizeVec4 = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(sizeVec4));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(2 * sizeVec4));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void *)(3 * sizeVec4));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int InstacnceColor;
    glGenBuffers(1, &InstacnceColor);
    glBindBuffer(GL_ARRAY_BUFFER, InstacnceColor);
    glBufferData(GL_ARRAY_BUFFER, num * sizeof(glm::vec3), colors, GL_STATIC_DRAW);
    for (unsigned int i = 0; i < CubeMesh.size(); ++i)
    {
        unsigned int currentVAO = CubeMesh[i].ServeVAO();
        glBindVertexArray(currentVAO);

        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);

        glVertexAttribDivisor(7, 1);
        glBindVertexArray(0);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Vars used for imgui
    bool grayscale = false;
    bool inversion = false;
    int kernel = 0;

    // Lights configs
    Light light(&HakuShader);
    glm::vec3 lightcol(1.0f, 1.0f, 1.0f);
    light.num_Dirlight += 1;
    glm::vec3 lightdir(-1.0f, -1.0f, -1.0f);
    HakuShader.Use();
    light.updateDirlight(0, lightdir, 0.6f * lightcol, 0.4f * lightcol, 0.6f * lightcol);

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

            ImGui::BulletText("Time:%.1fs", (float)glfwGetTime());
            ImGui::BulletText("FPS:%.1f", ImGui::GetIO().Framerate);
            ImGui::NewLine();
            ImGui::BulletText("Instance Items Rendered: %i", num);

            ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "PostEffects:");
            ImGui::NewLine();
            ImGui::Checkbox("Grayscale", &grayscale);
            ImGui::Checkbox("Inversion", &inversion);
            ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "Kernels Available:\n0::NoEffect\t1::Sharpen\t2::Blur\t3::EdgeDetection");
            ImGui::SliderInt("Kernel Selector", &kernel, 0, 3);

            ImGui::End();
        }

        ImGui::Render();

        glm::mat4 view = camera.GetViewMatrix();

        // UniformBlock Data Update
        glBindBuffer(GL_UNIFORM_BUFFER, MatricesBlock);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, fb.ID);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Render Code
        HakuShader.Use();
        Haku.Draw(&HakuShader);

        // Instance Rendering Code Here
        CubeShader.Use();
        Cube.DrawbyInstance(&CubeShader, num);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Imgui Dynamics
        fbShader.Use();
        fbShader.setBool("Grayscale", grayscale);
        fbShader.setBool("Inversion", inversion);
        fbShader.setInt("KernelIndex", kernel);

        fbShader.Use();
        glBindTexture(GL_TEXTURE_2D, fb.MultiSampledTexture2D());
        glBindVertexArray(fb.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    fb.Delete();
    glDeleteBuffers(1, &InstanceMatrices);
    glDeleteBuffers(1, &InstacnceColor);
    delete (Instancemodel);
    delete (colors);

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
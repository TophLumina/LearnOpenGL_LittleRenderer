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

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Shadow Mapping", NULL, NULL);
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
    FrameBuffer usualfb(ScreenWidth, ScreenHeight, 8);

    // Test Shader
    // Shader fbShader("./Shaders/OffScreen.vert", "./Shaders/offscreen_ForTesting.frag");
    Shader fbShader("./Shaders/OffScreen.vert", "./Shaders/OffScreen.frag");

    // Models and Shaders
    Model Haku("./Model/Haku/TDA Lacy Haku.pmx");
    // Shader HakuShader("./Shaders/Blinn_Phong.vert", "./Shaders/Blinn_Phong.frag");
    Shader HakuShader("./Shaders/ModelwithShadow.vert", "./Shaders/ModelwithShadow.frag");

    Model Floor("./Model/Floor/floor.fbx");
    Shader FloorShader("./Shaders/ModelwithShadow.vert", "./Shaders/ModelwithShadow.frag");

    Model Cube("./Model/JustCube/untitled.fbx");
    Shader CubeShader("./Shaders/instanceCube.vert", "./Shaders/instanceCube.frag");

    glm::mat4 model(1.0f);
    HakuShader.Use();
    HakuShader.setMat4("model", model);
    FloorShader.Use();
    FloorShader.setMat4("model", model);

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

    FloorShader.Use();
    unsigned int FLOOR_Matrices_Index = glGetUniformBlockIndex(FloorShader.ID, "Matrices");
    glUniformBlockBinding(FloorShader.ID, FLOOR_Matrices_Index, 0);

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
    bool gammacorrection = true;

    // Lights configs
    glm::vec3 lightcol(1.0f, 1.0f, 1.0f);
    glm::vec3 lightdir(-1.0f, -1.0f, -1.0f);
    Light Hakulight(&HakuShader);
    Hakulight.num_Dirlight += 1;
    HakuShader.Use();
    Hakulight.updateDirlight(0, lightdir, 0.6f * lightcol, 0.4f * lightcol, 0.6f * lightcol);

    Light Floorlight(&FloorShader);
    Floorlight.num_Dirlight += 1;
    FloorShader.Use();
    Floorlight.updateDirlight(0, lightdir, 0.6f * lightcol, 0.4f * lightcol, 0.6f * lightcol);

    // TODO::Add Shadow to the Scene
    // Create a DepthMask
    unsigned int DepthMapfbo;
    glGenFramebuffers(1, &DepthMapfbo);

    // Depth Map Texture Attachment
    // High Shadow Map Resolution means High Quality Shadow
    const unsigned int Shadow_Resolution = 4096;

    unsigned int Depth_Map;
    glGenTextures(1, &Depth_Map);
    glBindTexture(GL_TEXTURE_2D, Depth_Map);

    // Use the Depth Texture as a normal Texture and Sampling it
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Shadow_Resolution, Shadow_Resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    // Remove overborder samples
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float bordercolor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Binding
    glBindFramebuffer(GL_FRAMEBUFFER, DepthMapfbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, Depth_Map, 0);

    // and we don't need color attachment this time so disable the coloroutput of the framebuffer by setting its read/write buffer to NULL(GL_NONE aka 0)
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER::SHADOWMAPPING:: FrameBuffer is NOT Compelete." << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Matrices for Light Space Transform <only used for DirLight>
    // All Objects should be in the Space between far_plane and near_plane <Might need Refinements Here>
    const float near_plane = 30.0f;
    const float far_plane = 120.0f;
    float OrthoBorder = 10.0f;
    glm::mat4 Light_projection = glm::ortho(-OrthoBorder, OrthoBorder, -OrthoBorder, OrthoBorder, near_plane, far_plane);
    glm::vec3 DirLight_Pos = -25.0f * lightdir + 10.0f * camup;
    glm::mat4 Light_view = glm::lookAt(DirLight_Pos, DirLight_Pos + lightdir, camup);
    glm::mat4 Light_Space_Transform = Light_projection * Light_view;
    
    // Shadow Shader
    Shader ShadowShader("./Shaders/SimpleDepth.vert", "./Shaders/SimpleDepth.frag");
    ShadowShader.Use();
    ShadowShader.setMat4("LightSpaceTransform", Light_Space_Transform);

    // Static Lighting's Shadow Mapping
    glViewport(0, 0, Shadow_Resolution, Shadow_Resolution); // Shadow Map Resolution
    glBindFramebuffer(GL_FRAMEBUFFER, DepthMapfbo);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Peter Panning Fixed

    glCullFace(GL_FRONT);

    // Pre-Render
    ShadowShader.Use();
    ShadowShader.setMat4("model", model);
    ShadowShader.setBool("useInstance", false);
    Floor.Draw(&ShadowShader);
    Haku.Draw(&ShadowShader);

    // Set Culling Back
    // Need more complex logics to compute better shadow
    // glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Disable Cube Ring for Testing
    // ShadowShader.setBool("useInstance", true);
    // Cube.DrawbyInstance(&ShadowShader, num);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Model Shader Confirm
    HakuShader.Use();
    HakuShader.setMat4("LightSpaceTransform", Light_Space_Transform);
    // Unkown stuff :: GL_TEXTURE0 occupied?
    glActiveTexture(GL_TEXTURE1);
    HakuShader.setInt("Shadow_Map", 1);
    glBindTexture(GL_TEXTURE_2D, Depth_Map);

    FloorShader.Use();
    FloorShader.setMat4("LightSpaceTransform", Light_Space_Transform);
    glActiveTexture(GL_TEXTURE1);
    FloorShader.setInt("Shadow_Map", 1);
    glBindTexture(GL_TEXTURE_2D, Depth_Map);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Viewport Settings
    glViewport(0, 0, ScreenWidth, ScreenHeight);

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

            ImGui::NewLine();
            ImGui::TextColored(ImVec4(1.0, 0.0, 0.0, 1.0), "PostEffects:");
            
            ImGui::Checkbox("Grayscale", &grayscale);
            ImGui::Checkbox("Inversion", &inversion);
            ImGui::Checkbox("Gamma Correction", &gammacorrection);
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

        glBindFramebuffer(GL_FRAMEBUFFER, usualfb.ID);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Render Code
        HakuShader.Use();
        Haku.Draw(&HakuShader);

        FloorShader.Use();
        Floor.Draw(&FloorShader);

        // Instance Rendering Code Here
        CubeShader.Use();
        Cube.DrawbyInstance(&CubeShader, num);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        // Imgui Post Effects Dynamics
        fbShader.Use();
        fbShader.setBool("Grayscale", grayscale);
        fbShader.setBool("Inversion", inversion);
        fbShader.setInt("KernelIndex", kernel);
        fbShader.setBool("GammaCorrection", gammacorrection);

        fbShader.Use();

        // Test Texture
        glBindTexture(GL_TEXTURE_2D, usualfb.MultiSampledTexture2D());
        // glBindTexture(GL_TEXTURE_2D, Depth_Map);
        glBindVertexArray(usualfb.VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    usualfb.Delete();
    glDeleteBuffers(1, &InstanceMatrices);
    glDeleteBuffers(1, &InstacnceColor);
    delete (Instancemodel);
    delete (colors);

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}
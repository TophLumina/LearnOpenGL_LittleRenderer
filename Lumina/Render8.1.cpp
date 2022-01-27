#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
#include "CubewithNormalandTexcoords.hpp"

#include "Light.hpp"

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

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Advanced Light", NULL, NULL);
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Shader lightShader("./Shaders/7Vex.shader", "./Shaders/lightFag.shader");
    Shader objectShader("./Shaders/lightComplex.vert", "./Shaders/lightComplex.frag");

    //passing the texture to sampler in the shader
    int width = 800;
    int height = 800;
    int colChannel = 24;

    const char *loaderror = "Failed to load texture";
    stbi_set_flip_vertically_on_load(true);

    unsigned char *textureDatadiff = stbi_load("./Texture/Arknights.png", &width, &height, &colChannel, 0);
    if(!textureDatadiff)
        std::cout << loaderror << std::endl;

    unsigned int texturediff;
    glGenTextures(1, &texturediff);
    glBindTexture(GL_TEXTURE_2D, texturediff);

    //i'm lazy :)
    lazy::settextureformula();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureDatadiff);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(textureDatadiff);

    unsigned char *textureDataspec = stbi_load("./Texture/Arknights_specular.png", &width, &height, &colChannel, 0);
    if(!textureDataspec)
        std::cout << loaderror << std::endl;

    unsigned int texturespec;
    glGenTextures(1, &texturespec);
    glBindTexture(GL_TEXTURE_2D, texturespec);

    lazy::settextureformula();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureDataspec);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(textureDataspec);

    glBindTexture(GL_TEXTURE_2D, 0);

    //set the slot of the sampler
    objectShader.Use();
    objectShader.setInt("material.diffuse", 0);
    objectShader.setInt("material.specular", 1);

    //light test
    Light light(&objectShader);

    //pos for the cubes
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    //pos for pointlights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    // params used for UI
    bool dynamiclightsign = false;

    float lightcol[3] = {1.0f, 1.0f, 1.0f};
    float shinefactor = 32.0f;

    //lights
    light.num_Pointlight += 4;
    light.num_Spotlight += 1;

    while (!glfwWindowShouldClose(window)) {
        input(window);
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(debug_page) {
            ImGui::Begin("Debug Page");
            //stuff use to dynamicly debug
            ImGui::Checkbox("DynamicLightColor", &dynamiclightsign);
            ImGui::ColorEdit3("LightColor", lightcol);
            ImGui::NewLine();

            ImGui::SliderFloat("Shininess", &shinefactor, 32.0f, 512.0f, "%.0f");
            ImGui::NewLine();

            ImGui::BulletText("Camera Pos:(%.1f, %.1f, %.1f)", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::BulletText("Current Time: %.1fs", (float)glfwGetTime());
            ImGui::BulletText("FPS: %.1f", ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        glm::vec3 lightcolor(lightcol[0], lightcol[1], lightcol[2]);
        //changing color
        if(dynamiclightsign) {
            lightcolor.x = sin(glfwGetTime() * 2.0f);
            lightcolor.y = sin(glfwGetTime() * 0.7f);
            lightcolor.z = sin(glfwGetTime() * 1.3f);
        }

        lightShader.Use();
        lightShader.setVec3("lightColor", lightcolor);

        // and don't forget that the address of the shader was exposed only after the shader is used --<important>--
        objectShader.Use();
        objectShader.setFloat("material.shininess", shinefactor);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Fov), (float)ScreenWidth / (float)ScreenHeight, 0.1f, 100.0f);

        //need to activate texture unit every time before the Draw func is being called --<important>--
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturediff);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texturespec);

        lightShader.Use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glBindVertexArray(lightVAO);

        for(glm::vec3 pos : pointLightPositions) {
            glm::mat4 model(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        objectShader.Use();
        objectShader.setMat4("view",view);
        objectShader.setMat4("projection",projection);
        //the lightpos should be in view coords for we caculate lighting in the view space
        //test for class <Light>
        for (size_t i = 0; i < 4; i++)
            light.updatePointlight(i, glm::vec3(view * glm::vec4(pointLightPositions[i], 1.0f)), 0.1f * lightcolor, 0.5f * lightcolor, 1.0f * lightcolor, 1.0f, 0.09f, 0.032f);

        light.updateSpotlight(0, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.1f * lightcolor, 0.5f * lightcolor, 1.0f * lightcolor, 1.0f, 0.09f, 0.032f, 12.5f, 17.5f);

        glBindVertexArray(objectVAO);
        //10 cubes in different positions
        for(glm::vec3 pos : cubePositions) {
            glm::mat4 model(1.0f);
            model = glm::translate(model, pos);
            model = glm::rotate(model, (float)glm::radians(5.0f * glfwGetTime()), glm::vec3(1.0f, 0.5f, 0.3f));
            objectShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteTextures(1, &texturediff);
    glDeleteTextures(1, &texturespec);
    glDeleteBuffers(1, &lightVAO);
    glDeleteBuffers(1, &objectVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
}

//todo: Compelete different kinds of lights
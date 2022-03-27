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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void inputs(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float quad[] = {
    // Location      // Color
    -0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
    0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
    -0.05f, -0.05f, 0.0f, 0.0f, 1.0f,

    -0.05f, 0.05f, 1.0f, 0.0f, 0.0f,
    0.05f, -0.05f, 0.0f, 1.0f, 0.0f,
    0.05f, 0.05f, 0.0f, 1.0f, 1.0f};

int main()
{
    lazy::glfwCoreEnv(3, 3);
    int ScreenWidth = 800;
    int ScreenHeight = 600;

    GLFWwindow *window = glfwCreateWindow(ScreenWidth, ScreenHeight, "Instancing", NULL, NULL);
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

    // CallBacks
    glViewport(0, 0, ScreenWidth, ScreenHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Render Data
    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    Shader InstanceShader("./Shaders/Instance.vert", "./Shaders/Instance.frag");

    // Offsets
    glm::vec2 offsets[100];
    float defaultoffset = 0.0f;
    int index = 0;
    for (int x = -10; x < 10; x += 2)
    {
        for (int y = -10; y < 10; y += 2)
        {
            glm::vec2 offset;
            offset.x = (float)x / 10.0f + defaultoffset;
            offset.y = (float)y / 10.0f + defaultoffset;
            offsets[index++] = offset;
        }
    }

    // Using Instanced Array
    unsigned int InstancedArrayVBO;
    glGenBuffers(1, &InstancedArrayVBO);
    glBindBuffer(GL_ARRAY_BUFFER, InstancedArrayVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &offsets[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // InstanceShader.Use();
    // for(unsigned int i = 0; i < 100; ++i)
    // {
    //     std::string s = std::to_string(i);
    //     InstanceShader.setVec2(("offsets[" + s + "]").c_str(), offsets[i]);
    // }


    // AttribPointer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, InstancedArrayVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glVertexAttribDivisor(2, 1);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        inputs(window);
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);

        InstanceShader.Use();
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
}
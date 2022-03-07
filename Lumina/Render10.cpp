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
    
}
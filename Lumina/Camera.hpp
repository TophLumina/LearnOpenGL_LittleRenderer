#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

//Dirs of camera
enum CameraDir {
    FORWARD,
    BACKWARD,
    RIGHT,
    LEFT
};

//default pose of camera
const float PITCH = 0.0f;
const float YAW = -90.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;

class Camera {
    public:
    //Attribes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 AbsoluteUp;

    //angle
    float Pitch;
    float Yaw;

    //options
    float MovementSpeed;
    float TurningSensity;
    float Fov;

    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), TurningSensity(SENSITIVITY), Fov(FOV) {
        Position = pos;
        AbsoluteUp = up;
        Pitch = pitch;
        Yaw = yaw;

        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        glm::lookAt(Position, Position + Front, Up);
    }

    private:
    //update vectors
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, AbsoluteUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
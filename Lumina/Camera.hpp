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
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 45.0f;
const float z_near = 0.1f;
const float z_far = 100.0f;

class Camera {
    public:
    //Attribes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 AbsoluteUp;

    //eular angles
    float Pitch;
    float Yaw;

    //options
    float MovementSpeed;
    float TurningSensity;
    float Fov;
    float Znear;
    float Zfar;

    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float near = z_near, float far = z_far) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), TurningSensity(SENSITIVITY), Fov(FOV), Znear(near), Zfar(far) {
        Position = pos;
        AbsoluteUp = up;
        Pitch = pitch;
        Yaw = yaw;

        updateCameraVectors();
    }

    //by transfering metas
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) {
        Position = glm::vec3(posX, posY, posZ);
        AbsoluteUp = glm::vec3(upX, upY, upZ);
        Pitch = pitch;
        Yaw = yaw;

        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    //process keyboard input and unify with framerate
    void Move(CameraDir direction, float deltatime) {
        float velocity = MovementSpeed * deltatime;

        switch(direction) {
            case FORWARD:
                Position += Front * velocity;
                break;
            case BACKWARD:
                Position -= Front * velocity;
                break;
            case RIGHT:
                Position += Right * velocity;
                break;
            case LEFT:
                Position -= Right * velocity;
                break;
            default:;
        }
        //locked on xoz
        // Position.y = 0.0f;
    }

    void Mouse(float xoffset, float yoffset, GLboolean limitation = true) {
        xoffset *= TurningSensity;
        yoffset *= TurningSensity;

        Yaw += xoffset;
        Pitch += yoffset;

        if(limitation) {
            Pitch = Pitch > 89.0f ? 89.0f : Pitch;
            Pitch = Pitch < -89.0f ? -89.0f : Pitch; 
        }

        updateCameraVectors();
    }

    void MouseScroll(float yoffset) {
        Fov -= yoffset * 0.05f;
        Fov = Fov > 45.0f ? 45.0f : Fov;
        Fov = Fov < 1.0f ? 1.0f : Fov;
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
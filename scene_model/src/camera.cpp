#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

Camera::Camera(glm::vec3 position)
{
    Position = position;
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Yaw = -90.0f;
    Pitch = 0.0f;
    Speed = 2.5f;
    Sensitivity = 0.1f;
}

void Camera::ProcessKeyboard(int key, float deltaTime)
{
    float velocity = Speed * deltaTime;
    if (key == GLFW_KEY_W)
        Position += Front * velocity;
    if (key == GLFW_KEY_S)
        Position -= Front * velocity;
    if (key == GLFW_KEY_A)
        Position -= glm::normalize(glm::cross(Front, Up)) * velocity;
    if (key == GLFW_KEY_D)
        Position += glm::normalize(glm::cross(Front, Up)) * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    xoffset *= Sensitivity;
    yoffset *= Sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front.y = sin(glm::radians(Pitch));
    Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(Front);
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

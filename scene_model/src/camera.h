#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    float Yaw;
    float Pitch;
    float Speed;
    float Sensitivity;

    Camera(glm::vec3 position = glm::vec3(0, 0, 3));

    void ProcessKeyboard(int key, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    glm::mat4 GetViewMatrix();
};

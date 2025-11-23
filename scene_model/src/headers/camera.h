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

    Camera(glm::vec3 position = glm::vec3(-0.111342f, 0.173541f, -1.19045f));
    

    void ProcessKeyboard(int key, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    glm::mat4 GetViewMatrix();
};

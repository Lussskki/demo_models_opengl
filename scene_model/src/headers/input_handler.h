#pragma once
#include <GLFW/glfw3.h>
#include "camera.h"

class InputHandler
{
public:
    Camera* camera;
    float lastX, lastY;
    bool firstMouse;

    InputHandler(Camera* cam, float width, float height);

    void ProcessKeyboard(GLFWwindow* window, float deltaTime);
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);
};

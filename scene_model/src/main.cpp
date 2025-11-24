#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "headers/stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <cstddef>

#include "headers/camera.h"
#include "headers/input_handler.h"
#include "headers/debug_camera.h"
#include "model.h"
#include "cube.h"
#include "utils.h"
#include "skull_model.h"
#include "lion_model.h"
#include "cottage_model.h"
#include "tower_model.h" 

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 400;


std::string readFile(const std::string& path);
GLuint loadTexture(const std::string& path);
bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture);
GLuint compileShader(GLenum type, const char* code);
GLuint createShaderProgram(const char* vertexCode, const char* fragmentCode);

int main()
{
    // --- 1. GLFW Initialization ---
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Models", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    // --- 2. GLEW Initialization ---
    glewExperimental = true;
    if (glewInit() != GLEW_OK) { std::cerr << "GLEW init failed\n"; return -1; }

    glEnable(GL_DEPTH_TEST);

    // --- 3. Camera and Input Setup ---
    Camera camera;
    InputHandler input(&camera, SCR_WIDTH, SCR_HEIGHT);
    glfwSetWindowUserPointer(window, &input);
    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
        static_cast<InputHandler*>(glfwGetWindowUserPointer(w))->MouseCallback(w, x, y);
        });
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // --- 4. Shader Setup ---
    Cube cube(glm::vec3(-1.0f, 0.0f, -5.0f));
    std::string vertexCode = readFile("shaders/shader.vert");
    std::string fragmentCode = readFile("shaders/shader.frag");
    GLuint shaderProgram = createShaderProgram(vertexCode.c_str(), fragmentCode.c_str());

    // --- 5. Model Loading (Using TowerModel) ---

    // Tower model (extracted to object)
    TowerModel tower;
    if (!tower.Load("models/watch-tower-made-of-wood/wood_watch_tower2.obj"))
        std::cerr << "Failed to load Tower model\n";

    // Lion model (extracted to object)
    LionModel qilin;
    if (!qilin.Load("models/qilin/qilin.obj")) {
        std::cerr << "Failed to load Qilin model\n";
    }

    // Cottage model (extracted to object)
    CottageModel cottage;
    if (!cottage.Load("models/cottage/cottage_obj.obj"))
        std::cerr << "Failed to load Cottage model\n";

    // Skull Model (extracted to object)
    SkullModel skull;
    if (!skull.Load("models/skull/12140_Skull_v3_L2.obj")) {
        std::cerr << "Failed to load Skull model\n";
    }

    // --- 6. Model Positions and Settings ---
    glm::vec3 skullPos(-4.0f, 0.5f, -6.0f);
    float skullScale = 0.01f;
    float skullRotationY = 0.0f;

    glm::vec3 cubePos(-1.0f, 0.0f, -5.0f);

    glm::vec3 qilinPos(1.0f, 0.2f, -5.0f);
    float qilinScale = 0.0005f;
    float qilinRotationY = 0.0f;

    glm::vec3 cottagePos(4.0f, 0.0f, -3.0f);
    float cottageScale = 0.01f;
    float cottageRotationY = 80.0f;

    glm::vec3 towerPos(-12.0f, 0.0f, -18.0f);
    float towerScale = 2.0f;
    float towerRotationY = 0.0f;

    float lastFrame = 0.0f;
    bool f1_pressed_last_frame = false;
    glm::vec3 lightPos(2.0f, 4.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    // ---------------- Main loop ----------------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- 7. Input Processing ---
        input.ProcessKeyboard(window, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            DebugCameraInfo(
                camera,
                cubePos,
                qilinPos, qilinScale, qilinRotationY,
                cottagePos, cottageScale, cottageRotationY,
                skullPos, skullScale, skullRotationY,
                towerPos, towerScale, towerRotationY
            );
        }

        bool f1_currently_pressed = glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS;
        if (f1_currently_pressed && !f1_pressed_last_frame)
        {
            PromptForNewPositions(
                cubePos,
                qilinPos, qilinScale, qilinRotationY,
                cottagePos, cottageScale, cottageRotationY,
                skullPos, skullScale, skullRotationY,
                towerPos, towerScale, towerRotationY
            );
        }
        f1_pressed_last_frame = f1_currently_pressed;

        // --- 8. Rendering Setup ---
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        // Set Shader Uniforms
        GLint locLightPos = glGetUniformLocation(shaderProgram, "lightPos");
        if (locLightPos >= 0) glUniform3fv(locLightPos, 1, glm::value_ptr(lightPos));
        GLint locLightColor = glGetUniformLocation(shaderProgram, "lightColor");
        if (locLightColor >= 0) glUniform3fv(locLightColor, 1, glm::value_ptr(lightColor));
        GLint locViewPos = glGetUniformLocation(shaderProgram, "viewPos");
        if (locViewPos >= 0) glUniform3fv(locViewPos, 1, glm::value_ptr(camera.Position));

        GLint locView = glGetUniformLocation(shaderProgram, "view");
        if (locView >= 0) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));
        GLint locProj = glGetUniformLocation(shaderProgram, "projection");
        if (locProj >= 0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));
        GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture"); // Pass to Cube draw

        // --- 9. Draw Models ---

        // Draw Tower (using the class)
        tower.Draw(shaderProgram, towerPos, towerScale, towerRotationY);

        // Draw Cube
        glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), cubePos);
        GLint locModel = glGetUniformLocation(shaderProgram, "model");
        if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelCube));
        cube.Draw(shaderProgram, locModel, locUseTex);

        // Draw other models
        cottage.Draw(shaderProgram, cottagePos, cottageScale, cottageRotationY);
        qilin.Draw(shaderProgram, qilinPos, qilinScale, qilinRotationY);
        skull.Draw(shaderProgram, skullPos, skullScale, skullRotationY);

        // --- 10. End of Frame ---
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
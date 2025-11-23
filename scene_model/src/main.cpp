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

#include "headers/camera.h"
#include "headers/input_handler.h"


#include <cstddef>

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 400;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color;
    glm::vec2 TexCoords;
};

std::string readFile(const std::string& path);
GLuint loadTexture(const std::string& path);
bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture);
GLuint compileShader(GLenum type, const char* code);
GLuint createShaderProgram(const char* vertexCode, const char* fragmentCode);

void DebugCameraInfo(
    const Camera& camera,
    const glm::vec3& cubePos,
    const glm::vec3& qilinPos, float qilinScale, float qilinRotationY,
    const glm::vec3& cottagePos, float cottageScale, float cottageRotationY,
    const glm::vec3& skullPos, float skullScale, float skullRotationY,
    const glm::vec3& towerPos, float towerScale, float towerRotationY
);

void PromptForNewPositions(
    glm::vec3& cubePos,
    glm::vec3& qilinPos, float& qilinScale, float& qilinRotationY,
    glm::vec3& cottagePos, float& cottageScale, float& cottageRotationY,
    glm::vec3& skullPos, float& skullScale, float& skullRotationY,
    glm::vec3& towerPos, float& towerScale, float& towerRotationY
);


void DebugCameraInfo(
    const Camera& camera,
    const glm::vec3& cubePos,
    const glm::vec3& qilinPos, float qilinScale, float qilinRotationY,
    const glm::vec3& cottagePos, float cottageScale, float cottageRotationY,
    const glm::vec3& skullPos, float skullScale, float skullRotationY,
    const glm::vec3& towerPos, float towerScale, float towerRotationY
)
{
    glm::vec3 pos = camera.Position;
    glm::vec3 front = camera.Front;

    std::cout << "\n--- Camera Info ---\n";
    std::cout << "Position (X, Y, Z): "
        << pos.x << ", " << pos.y << ", " << pos.z
        << "\nFront Vector (Dir): "
        << front.x << ", " << front.y << ", " << front.z
        << "\n";

    std::cout << "--- Model Positions ---\n";

    std::cout << "Cube Pos:        ("
        << cubePos.x << ", " << cubePos.y << ", " << cubePos.z << ")\n";

    std::cout << "Qilin Pos/Scale/Rot:   ("
        << qilinPos.x << ", " << qilinPos.y << ", " << qilinPos.z << ") | Scale: "
        << qilinScale << " | Rot Y: " << qilinRotationY << "\n";

    std::cout << "Cottage Pos/Scale/Rot: ("
        << cottagePos.x << ", " << cottagePos.y << ", " << cottagePos.z << ") | Scale: "
        << cottageScale << " | Rot Y: " << cottageRotationY << "\n";

    std::cout << "Skull Pos/Scale/Rot:   ("
        << skullPos.x << ", " << skullPos.y << ", " << skullPos.z << ") | Scale: "
        << skullScale << " | Rot Y: " << skullRotationY << "\n";

    std::cout << "Tower Pos/Scale/Rot:   ("
        << towerPos.x << ", " << towerPos.y << ", " << towerPos.z << ") | Scale: "
        << towerScale << " | Rot Y: " << towerRotationY << "\n";
    std::cout << "-----------------------\n";
}

void PromptForNewPositions(
    glm::vec3& cubePos,
    glm::vec3& qilinPos, float& qilinScale, float& qilinRotationY,
    glm::vec3& cottagePos, float& cottageScale, float& cottageRotationY,
    glm::vec3& skullPos, float& skullScale, float& skullRotationY,
    glm::vec3& towerPos, float& towerScale, float& towerRotationY
)
{
    auto updateModel = [](const std::string& modelName, glm::vec3& pos, float& scale, float& rotY) {
        std::cout << "\n--- Editing " << modelName << " ---\n";

        std::cout << "Current Pos (X Y Z): " << pos.x << " " << pos.y << " " << pos.z << "\n";
        std::cout << "Enter NEW Position (X Y Z): ";
        float x, y, z;
        if (!(std::cin >> x >> y >> z)) {
            std::cerr << "Invalid input for " << modelName << " position. Skipping update.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        pos = glm::vec3(x, y, z);

        std::cout << "Current Scale: " << scale << "\n";
        std::cout << "Enter NEW Scale: ";
        float s;
        if (!(std::cin >> s)) {
            std::cerr << "Invalid input for " << modelName << " scale. Skipping update.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        scale = s;

        std::cout << "Current Rotation Y (degrees): " << rotY << "\n";
        std::cout << "Enter NEW Rotation Y (degrees): ";
        float r;
        if (!(std::cin >> r)) {
            std::cerr << "Invalid input for " << modelName << " Y-rotation. Skipping update.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        rotY = r;
        };

    auto updateCube = [](glm::vec3& pos) {
        std::cout << "\n--- Editing Cube ---\n";
        std::cout << "Current Pos (X Y Z): " << pos.x << " " << pos.y << " " << pos.z << "\n";
        std::cout << "Enter NEW Position (X Y Z): ";
        float x, y, z;
        if (!(std::cin >> x >> y >> z)) {
            std::cerr << "Invalid input for Cube position. Skipping update.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return;
        }
        pos = glm::vec3(x, y, z);
        };

    std::cout << "\n=========================================\n";
    std::cout << "!!! INTERACTIVE EDIT MODE ACTIVATED !!!\n";
    std::cout << "The rendering window will be BLOCKED until input is complete.\n";

    updateCube(cubePos);
    updateModel("Qilin", qilinPos, qilinScale, qilinRotationY);
    updateModel("Cottage", cottagePos, cottageScale, cottageRotationY);
    updateModel("Skull", skullPos, skullScale, skullRotationY);
    updateModel("Tower", towerPos, towerScale, towerRotationY);

    std::cout << "!!! EDITING COMPLETE. Resuming rendering. !!!\n";
    std::cout << "=========================================\n";
}


int main()
{
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Models", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) { std::cerr << "GLEW init failed\n"; return -1; }

    glEnable(GL_DEPTH_TEST);

    Camera camera;
    InputHandler input(&camera, SCR_WIDTH, SCR_HEIGHT);
    glfwSetWindowUserPointer(window, &input);
    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
        static_cast<InputHandler*>(glfwGetWindowUserPointer(w))->MouseCallback(w, x, y);
        });
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float cubeVertices[] =
    {
        -0.5f,-0.5f,-0.5f,        1,0,0,
         0.5f,-0.5f,-0.5f,        0,1,0,
         0.5f, 0.5f,-0.5f,        0,0,1,
         0.5f, 0.5f,-0.5f,        0,0,1,
        -0.5f, 0.5f,-0.5f,        1,1,0,
        -0.5f,-0.5f,-0.5f,        1,0,0,

        -0.5f,-0.5f, 0.5f,        1,0,1,
         0.5f,-0.5f, 0.5f,        0,1,1,
         0.5f, 0.5f, 0.5f,        1,1,1,
         0.5f, 0.5f, 0.5f,        1,1,1,
        -0.5f, 0.5f, 0.5f,        1,0,0,
        -0.5f,-0.5f, 0.5f,        1,0,1,

        -0.5f, 0.5f, 0.5f,        0,1,0,
        -0.5f, 0.5f,-0.5f,        0,0,1,
        -0.5f,-0.5f,-0.5f,        1,0,0,
        -0.5f,-0.5f,-0.5f,        1,0,0,
        -0.5f,-0.5f, 0.5f,        1,1,0,
        -0.5f, 0.5f, 0.5f,        0,1,0,

         0.5f, 0.5f, 0.5f,        1,0,1,
         0.5f, 0.5f,-0.5f,        0,1,1,
         0.5f,-0.5f,-0.5f,        0,0,1,
         0.5f,-0.5f,-0.5f,        0,0,1,
         0.5f,-0.5f, 0.5f,        1,1,1,
         0.5f, 0.5f, 0.5f,        1,0,1,

        -0.5f,-0.5f,-0.5f,        1,0,0,
         0.5f,-0.5f,-0.5f,        0,1,0,
         0.5f,-0.5f, 0.5f,        0,0,1,
         0.5f,-0.5f, 0.5f,        0,0,1,
        -0.5f,-0.5f, 0.5f,        1,1,0,
        -0.5f,-0.5f,-0.5f,        1,0,0,

        -0.5f, 0.5f,-0.5f,        0,1,1,
         0.5f, 0.5f,-0.5f,        1,0,1,
         0.5f, 0.5f, 0.5f,        0,1,0,
         0.5f, 0.5f, 0.5f,        0,1,0,
        -0.5f, 0.5f, 0.5f,        1,1,1,
        -0.5f, 0.5f,-0.5f,        0,1,1
    };

    GLuint VAO_cube, VBO_cube;
    glGenVertexArrays(1, &VAO_cube);
    glGenBuffers(1, &VBO_cube);
    glBindVertexArray(VAO_cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    GLsizei cubeStride = 6 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride, (void*)0);
    glEnableVertexAttribArray(0);

    glDisableVertexAttribArray(1);
    glVertexAttrib3f(1, 0.0f, 0.0f, 1.0f);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, cubeStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glDisableVertexAttribArray(3);
    glVertexAttrib2f(3, 0.0f, 0.0f);

    glBindVertexArray(0);

    std::string vertexCode = readFile("shaders/shader.vert");
    std::string fragmentCode = readFile("shaders/shader.frag");

    std::cout << "Vertex size: " << vertexCode.size() << "\n";
    std::cout << "Fragment size: " << fragmentCode.size() << "\n";

    GLuint shaderProgram = createShaderProgram(vertexCode.c_str(), fragmentCode.c_str());

    std::vector<Vertex> towerVertices;
    GLuint towerTexture;
    if (!loadOBJ("models/watch-tower-made-of-wood/wood_watch_tower2.obj", towerVertices, towerTexture))
        std::cerr << "Failed to load Tower OBJ\n";

    GLuint VAO_tower = 0, VBO_tower = 0;
    if (!towerVertices.empty())
    {
        glGenVertexArrays(1, &VAO_tower);
        glGenBuffers(1, &VBO_tower);
        glBindVertexArray(VAO_tower);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_tower);
        glBufferData(GL_ARRAY_BUFFER, towerVertices.size() * sizeof(Vertex), towerVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    std::vector<Vertex> qilinVertices;
    GLuint qilinTexture;
    if (!loadOBJ("models/qilin/qilin.obj", qilinVertices, qilinTexture))
        std::cerr << "Failed to load Qilin OBJ\n";

    GLuint VAO_qilin = 0, VBO_qilin = 0;
    if (!qilinVertices.empty())
    {
        glGenVertexArrays(1, &VAO_qilin);
        glGenBuffers(1, &VBO_qilin);
        glBindVertexArray(VAO_qilin);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_qilin);
        glBufferData(GL_ARRAY_BUFFER, qilinVertices.size() * sizeof(Vertex), qilinVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    std::vector<Vertex> cottageVertices;
    GLuint cottageTexture;
    if (!loadOBJ("models/cottage/cottage_obj.obj", cottageVertices, cottageTexture))
        std::cerr << "Failed to load Cottage OBJ\n";

    GLuint VAO_cottage = 0, VBO_cottage = 0;
    if (!cottageVertices.empty())
    {
        glGenVertexArrays(1, &VAO_cottage);
        glGenBuffers(1, &VBO_cottage);
        glBindVertexArray(VAO_cottage);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_cottage);
        glBufferData(GL_ARRAY_BUFFER, cottageVertices.size() * sizeof(Vertex), cottageVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    std::vector<Vertex> skullVertices;
    GLuint skullTexture;
    if (!loadOBJ("models/skull/12140_Skull_v3_L2.obj", skullVertices, skullTexture))
        std::cerr << "Failed to load Skull OBJ\n";

    GLuint VAO_skull = 0, VBO_skull = 0;
    if (!skullVertices.empty())
    {
        glGenVertexArrays(1, &VAO_skull);
        glGenBuffers(1, &VBO_skull);
        glBindVertexArray(VAO_skull);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_skull);
        glBufferData(GL_ARRAY_BUFFER, skullVertices.size() * sizeof(Vertex), skullVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }
    glm::vec3 cubePos = glm::vec3(-1.0f, 0.0f, -5.0f);

    glm::vec3 qilinPos = glm::vec3(1.0f, 0.2f, -5.0f);
    float qilinScale = 0.0005f;
    float qilinRotationY = 0.0f;

    glm::vec3 cottagePos = glm::vec3(4.0f, 0.0f, -3.0f);
    float cottageScale = 0.01f;
    float cottageRotationY = 80.0f;

    glm::vec3 skullPos = glm::vec3(-4.0f, 0.5f, -6.0f);
    float skullScale = 0.01f;
    float skullRotationY = 0.0f;

    glm::vec3 towerPos = glm::vec3(-12.0f, 0.0f, -18.0f);
    float towerScale = 2.0f;
    float towerRotationY = 0.0f;

    float lastFrame = 0.0f;

    bool f1_pressed_last_frame = false;

    glm::vec3 lightPos(2.0f, 4.0f, 2.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        GLint locLightPos = glGetUniformLocation(shaderProgram, "lightPos");
        if (locLightPos >= 0) glUniform3fv(locLightPos, 1, glm::value_ptr(lightPos));
        GLint locLightColor = glGetUniformLocation(shaderProgram, "lightColor");
        if (locLightColor >= 0) glUniform3fv(locLightColor, 1, glm::value_ptr(lightColor));
        GLint locViewPos = glGetUniformLocation(shaderProgram, "viewPos");
        if (locViewPos >= 0) {
            glUniform3fv(locViewPos, 1, glm::value_ptr(camera.Position));
        }

        GLint locView = glGetUniformLocation(shaderProgram, "view");
        if (locView >= 0) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));
        GLint locProj = glGetUniformLocation(shaderProgram, "projection");
        if (locProj >= 0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));
        GLint locModel = glGetUniformLocation(shaderProgram, "model");
        GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture");


        glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), cubePos);
        if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelCube));
        if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);

        glBindVertexArray(VAO_cube);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);


        if (VAO_tower)
        {
            glm::mat4 modelTower = glm::translate(glm::mat4(1.0f), towerPos);
            modelTower = glm::rotate(modelTower, glm::radians(towerRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelTower = glm::scale(modelTower, glm::vec3(towerScale));

            if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelTower));

            if (towerTexture != 0) {
                if (locUseTex >= 0) glUniform1i(locUseTex, GL_TRUE);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, towerTexture);
                GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
                if (texLoc >= 0) glUniform1i(texLoc, 0);
            }
            else {
                if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);
            }

            glBindVertexArray(VAO_tower);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)towerVertices.size());
            glBindVertexArray(0);
        }


        if (VAO_qilin)
        {
            glm::mat4 modelQilin = glm::translate(glm::mat4(1.0f), qilinPos);

            modelQilin = glm::rotate(modelQilin, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelQilin = glm::rotate(modelQilin, glm::radians(-300.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            modelQilin = glm::rotate(modelQilin, glm::radians(qilinRotationY), glm::vec3(0.0f, 1.0f, 0.0f));

            modelQilin = glm::scale(modelQilin, glm::vec3(qilinScale));

            if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelQilin));

            if (qilinTexture != 0) {
                if (locUseTex >= 0) glUniform1i(locUseTex, GL_TRUE);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, qilinTexture);
                GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
                if (texLoc >= 0) glUniform1i(texLoc, 0);
            }
            else {
                if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);
            }

            glBindVertexArray(VAO_qilin);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)qilinVertices.size());
            glBindVertexArray(0);
        }

        if (VAO_cottage)
        {
            glm::mat4 modelCottage = glm::translate(glm::mat4(1.0f), cottagePos);
            modelCottage = glm::rotate(modelCottage, glm::radians(cottageRotationY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelCottage = glm::scale(modelCottage, glm::vec3(cottageScale));

            if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelCottage));

            if (cottageTexture != 0) {
                if (locUseTex >= 0) glUniform1i(locUseTex, GL_TRUE);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, cottageTexture);
                GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
                if (texLoc >= 0) glUniform1i(texLoc, 0);
            }
            else {
                if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);
            }

            glBindVertexArray(VAO_cottage);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)cottageVertices.size());
            glBindVertexArray(0);
        }

        if (VAO_skull)
        {
            glm::mat4 modelSkull = glm::translate(glm::mat4(1.0f), skullPos);
            modelSkull = glm::rotate(modelSkull, glm::radians(-90.0f), glm::vec3(2.0f, 0.0f, 0.0f));
            modelSkull = glm::rotate(modelSkull, glm::radians(360.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            modelSkull = glm::rotate(modelSkull, glm::radians(skullRotationY), glm::vec3(0.0f, 1.0f, 0.0f));

            modelSkull = glm::scale(modelSkull, glm::vec3(skullScale));
            if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelSkull));

            if (skullTexture != 0) {
                if (locUseTex >= 0) glUniform1i(locUseTex, GL_TRUE);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, skullTexture);
                GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
                if (texLoc >= 0) glUniform1i(texLoc, 0);
            }
            else {
                if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);
            }

            glBindVertexArray(VAO_skull);
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)skullVertices.size());
            glBindVertexArray(0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO_cube);
    glDeleteBuffers(1, &VBO_cube);

    if (VAO_tower) { glDeleteVertexArrays(1, &VAO_tower); glDeleteBuffers(1, &VBO_tower); }
    if (VAO_qilin) { glDeleteVertexArrays(1, &VAO_qilin); glDeleteBuffers(1, &VBO_qilin); }
    if (VAO_cottage) { glDeleteVertexArrays(1, &VAO_cottage); glDeleteBuffers(1, &VBO_cottage); }
    if (VAO_skull) { glDeleteVertexArrays(1, &VAO_skull); glDeleteBuffers(1, &VBO_skull); }

    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

std::string readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint loadTexture(const std::string& path)
{
    GLuint textureID = 0;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3 ? GL_RGB : GL_RED);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        if (data) stbi_image_free(data);
        textureID = 0;
    }
    return textureID;
}

bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string baseDir = path.substr(0, path.find_last_of("/\\") + 1);

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), baseDir.c_str(), true);
    if (!ret)
    {
        std::cerr << "Failed to load OBJ: " << err << std::endl;
        return false;
    }
    if (!warn.empty()) std::cout << "OBJ Warning: " << warn << std::endl;

    outTexture = 0;
    if (!materials.empty() && !materials[0].diffuse_texname.empty())
    {
        std::string texturePath = baseDir + materials[0].diffuse_texname;
        outTexture = loadTexture(texturePath);
        if (outTexture == 0) std::cout << "Texture '" << materials[0].diffuse_texname << "' not found, using vertex colors\n";
    }

    std::cout << "Loaded OBJ: " << path << " (" << attrib.vertices.size() / 3 << " vertices)\n";


    bool hasNormals = !attrib.normals.empty();
    for (const auto& shape : shapes)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f];
            int matID = (shape.mesh.material_ids.size() > f) ? shape.mesh.material_ids[f] : -1;
            glm::vec3 faceColor(0.8f, 0.8f, 0.8f);
            if (matID >= 0 && matID < (int)materials.size()) {
                faceColor = glm::vec3(materials[matID].diffuse[0], materials[matID].diffuse[1], materials[matID].diffuse[2]);
            }

            glm::vec3 computedNormal(0.0f);
            if (!hasNormals && fv >= 3)
            {
                tinyobj::index_t idx0 = shape.mesh.indices[index_offset + 0];
                tinyobj::index_t idx1 = shape.mesh.indices[index_offset + 1];
                tinyobj::index_t idx2 = shape.mesh.indices[index_offset + 2];
                glm::vec3 v0(
                    attrib.vertices[3 * idx0.vertex_index + 0],
                    attrib.vertices[3 * idx0.vertex_index + 1],
                    attrib.vertices[3 * idx0.vertex_index + 2]
                );
                glm::vec3 v1(
                    attrib.vertices[3 * idx1.vertex_index + 0],
                    attrib.vertices[3 * idx1.vertex_index + 1],
                    attrib.vertices[3 * idx1.vertex_index + 2]
                );
                glm::vec3 v2(
                    attrib.vertices[3 * idx2.vertex_index + 0],
                    attrib.vertices[3 * idx2.vertex_index + 1],
                    attrib.vertices[3 * idx2.vertex_index + 2]
                );
                computedNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            }

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                Vertex vertex;
                vertex.Position = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                };

                if (idx.normal_index >= 0 && hasNormals)
                {
                    vertex.Normal = {
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                }
                else {
                    vertex.Normal = computedNormal;
                }

                vertex.Color = faceColor;

                if (idx.texcoord_index >= 0 && !attrib.texcoords.empty())
                {
                    vertex.TexCoords = {
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    };
                }
                else {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }

                outVertices.push_back(vertex);
            }
            index_offset += fv;
        }
    }

    return true;
}

GLuint compileShader(GLenum type, const char* code) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compile error: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexCode, const char* fragmentCode) {
    GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    int success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}
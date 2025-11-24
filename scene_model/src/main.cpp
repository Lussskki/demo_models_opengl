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

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 400;

std::string readFile(const std::string& path);
GLuint loadTexture(const std::string& path);
bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture);
GLuint compileShader(GLenum type, const char* code);
GLuint createShaderProgram(const char* vertexCode, const char* fragmentCode);

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

    Cube myCube(glm::vec3(-1.0f, 0.0f, -5.0f));

    std::string vertexCode = readFile("shaders/shader.vert");
    std::string fragmentCode = readFile("shaders/shader.frag");
    GLuint shaderProgram = createShaderProgram(vertexCode.c_str(), fragmentCode.c_str());

    // ---------------- Load other models ----------------
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

    // ---------------- Skull Model ----------------
    SkullModel skull;
    glm::vec3 skullPos(-4.0f, 0.5f, -6.0f);
    float skullScale = 0.01f;
    float skullRotationY = 0.0f;

    if (!skull.Load("models/skull/12140_Skull_v3_L2.obj")) {
        std::cerr << "Failed to load Skull model\n";
    }

    // ---------------- Other model positions ----------------
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
        if (locViewPos >= 0) glUniform3fv(locViewPos, 1, glm::value_ptr(camera.Position));

        GLint locView = glGetUniformLocation(shaderProgram, "view");
        if (locView >= 0) glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));
        GLint locProj = glGetUniformLocation(shaderProgram, "projection");
        if (locProj >= 0) glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));
        GLint locModel = glGetUniformLocation(shaderProgram, "model");
        GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture");

        // Draw models
        myCube.Draw(shaderProgram, locModel, locUseTex);

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

        // Draw the skull
        skull.Draw(shaderProgram, skullPos, skullScale, skullRotationY);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    if (VAO_tower) { glDeleteVertexArrays(1, &VAO_tower); glDeleteBuffers(1, &VBO_tower); }
    if (VAO_qilin) { glDeleteVertexArrays(1, &VAO_qilin); glDeleteBuffers(1, &VBO_qilin); }
    if (VAO_cottage) { glDeleteVertexArrays(1, &VAO_cottage); glDeleteBuffers(1, &VBO_cottage); }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

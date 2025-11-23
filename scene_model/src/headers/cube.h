#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Cube
{
    GLuint VAO, VBO;
    glm::vec3 position;

    Cube(const glm::vec3& pos = glm::vec3(0.0f));
    ~Cube();

    void Draw(GLuint shaderProgram, GLint locModel, GLint locUseTex);
};


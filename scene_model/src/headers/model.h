#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Color;
    glm::vec2 TexCoords;
};

class Model {
public:
    Model(const std::string& path);
    void Draw(GLuint shaderProgram);

private:
    std::vector<Vertex> vertices;
    GLuint VAO, VBO;
    void loadModel(const std::string& path);
};

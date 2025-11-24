#ifndef SKULL_MODEL_H
#define SKULL_MODEL_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "model.h" // Vertex struct

class SkullModel
{
public:
    SkullModel() : VAO(0), VBO(0), texture(0) {}
    ~SkullModel();

    bool Load(const std::string& objPath, const std::string& texturePath = "");
    void Draw(GLuint shaderProgram, const glm::vec3& position, float scale, float rotationY);

private:
    std::vector<Vertex> vertices;
    GLuint texture;
    GLuint VAO, VBO;
};

#endif

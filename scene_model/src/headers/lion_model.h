#ifndef LION_MODEL_H
#define LION_MODEL_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "model.h"        // for Vertex struct
#include "utils.h"        // loadOBJ, loadTexture

class LionModel
{
public:
    LionModel();
    ~LionModel();

    bool Load(const std::string& objPath);

    void Draw(GLuint shaderProgram,
        const glm::vec3& position,
        float scale,
        float rotationY);

private:
    GLuint VAO, VBO;
    GLuint textureID;
    std::vector<Vertex> vertices;

    void SetupMesh();
};

#endif

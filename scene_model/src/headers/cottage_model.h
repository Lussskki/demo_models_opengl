
#pragma once

#include <string>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "model.h" // must define Vertex

class CottageModel {
public:
    CottageModel();
    ~CottageModel();

    // Load model from OBJ path (uses existing loadOBJ helper)
    // returns true on success
    bool Load(const std::string& objPath);

    // Draw the cottage. shaderProgram should be the currently used shader.
    // position, scale, rotationY as in main scene code.
    void Draw(GLuint shaderProgram, const glm::vec3& position, float scale, float rotationY);

    // optional: free GPU resources explicitly
    void Unload();

private:
    GLuint VAO;
    GLuint VBO;
    GLuint texture;      // 0 means no texture
    GLsizei vertexCount;
    std::vector<Vertex> vertices;
    bool initialized;
};

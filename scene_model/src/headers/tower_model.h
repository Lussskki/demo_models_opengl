#ifndef TOWER_MODEL_H
#define TOWER_MODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include "model.h" // for Vertex
#include "tiny_obj_loader.h" // <- NO IMPLEMENTATION HERE
#include "stb_image.h"        // <- NO IMPLEMENTATION HERE

class TowerModel
{
public:
    TowerModel();
    ~TowerModel();

    bool Load(const std::string& objPath, const std::string& texturePath = "");
    void Draw(GLuint shaderProgram, const glm::vec3& position, float scale = 1.0f, float rotationY = 0.0f);

private:
    std::vector<Vertex> vertices;
    GLuint texture;
    GLuint VAO, VBO;

    bool loadOBJ(const std::string& path);
    GLuint loadTextureFromFile(const std::string& path);
};

#endif

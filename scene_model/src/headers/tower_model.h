#ifndef TOWER_MODEL_H
#define TOWER_MODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>


struct Vertex; 

extern bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture);

class TowerModel {
public:
    TowerModel();
    ~TowerModel();

    bool Load(const std::string& objPath);
    void Draw(GLuint shaderProgram, const glm::vec3& position, float scale, float rotationY) const;

private:
    std::vector<Vertex> m_vertices;
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_textureID;
    TowerModel(const TowerModel&) = delete;
    TowerModel& operator=(const TowerModel&) = delete;
};

#endif 
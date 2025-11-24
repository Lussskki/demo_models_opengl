#include "cottage_model.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// These functions are defined elsewhere in your project (same as in main.cpp)
extern bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture);
extern GLuint loadTexture(const std::string& path); // if you ever need it

CottageModel::CottageModel()
    : VAO(0), VBO(0), texture(0), vertexCount(0), initialized(false)
{
}

CottageModel::~CottageModel()
{
    Unload();
}

void CottageModel::Unload()
{
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    if (VBO) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (texture) {
        glDeleteTextures(1, &texture);
        texture = 0;
    }
    vertices.clear();
    vertexCount = 0;
    initialized = false;
}

bool CottageModel::Load(const std::string& objPath)
{
    Unload();

    GLuint tex = 0;
    if (!loadOBJ(objPath, vertices, tex)) {
        std::cerr << "CottageModel::Load - loadOBJ failed for: " << objPath << "\n";
        return false;
    }

    if (vertices.empty()) {
        std::cerr << "CottageModel::Load - no vertices loaded for: " << objPath << "\n";
        return false;
    }

    texture = tex;
    vertexCount = static_cast<GLsizei>(vertices.size());

    // create VAO/VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // position (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));

    // normal (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // color (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

    // tex coords (location = 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    initialized = true;
    return true;
}

void CottageModel::Draw(GLuint shaderProgram, const glm::vec3& position, float scale, float rotationY)
{
    if (!initialized || VAO == 0 || vertexCount == 0) return;

    // build model matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale));

    GLint locModel = glGetUniformLocation(shaderProgram, "model");
    if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));

    GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture");
    if (texture != 0) {
        if (locUseTex >= 0) glUniform1i(locUseTex, GL_TRUE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
        if (texLoc >= 0) glUniform1i(texLoc, 0);
    }
    else {
        if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    // unbind texture to be safe
    if (texture != 0) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

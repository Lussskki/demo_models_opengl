#include "tower_model.h"
#include "utils.h" 

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cstddef> 

TowerModel::TowerModel() : m_VAO(0), m_VBO(0), m_textureID(0) {}

TowerModel::~TowerModel() {
    if (m_VAO) {
        glDeleteVertexArrays(1, &m_VAO);
    }
    if (m_VBO) {
        glDeleteBuffers(1, &m_VBO);
    }
}

bool TowerModel::Load(const std::string& objPath) {
    // 1. Load the OBJ data
    if (!loadOBJ(objPath, m_vertices, m_textureID)) {
        std::cerr << "Failed to load Tower OBJ from: " << objPath << std::endl;
        return false;
    }

    if (m_vertices.empty()) {
        std::cerr << "Tower OBJ loaded but contains no vertices." << std::endl;
        return false;
    }

    // 2. Setup OpenGL buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), m_vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);

    // Color attribute (optional, based on your shader)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(2);

    // Texture coordinate attribute
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0); // Unbind VAO

    return true;
}

void TowerModel::Draw(GLuint shaderProgram, const glm::vec3& position, float scale, float rotationY) const {
    if (m_VAO == 0) {
        return;
    }

    // 1. Compute Model Matrix
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scale));

    // 2. Send Model Matrix to shader
    GLint locModel = glGetUniformLocation(shaderProgram, "model");
    if (locModel >= 0) {
        glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
    }

    // 3. Handle Texture binding and uniform
    GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture");
    if (m_textureID != 0) {
        if (locUseTex >= 0) glUniform1i(locUseTex, GL_TRUE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textureID);
        GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
        if (texLoc >= 0) glUniform1i(texLoc, 0); // Assumes texture sampler is named "texture1"
    }
    else {
        if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);
    }

    // 4. Draw
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());
    glBindVertexArray(0);

    // Unbind texture (optional, but good practice)
    if (m_textureID != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}
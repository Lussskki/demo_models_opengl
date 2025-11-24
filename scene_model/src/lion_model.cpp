#include "lion_model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

LionModel::LionModel()
{
    VAO = 0;
    VBO = 0;
    textureID = 0;
}

LionModel::~LionModel()
{
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

bool LionModel::Load(const std::string& objPath)
{
    GLuint tex;
    if (!loadOBJ(objPath, vertices, tex))
    {
        printf("LionModel: Failed to load OBJ: %s\n", objPath.c_str());
        return false;
    }

    textureID = tex;

    SetupMesh();
    return true;
}

void LionModel::SetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

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

void LionModel::Draw(GLuint shaderProgram,
    const glm::vec3& position,
    float scale,
    float rotationY)
{
    if (!VAO) return;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

    // Qilin needs rotation 
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(-300.0f), glm::vec3(0, 0, 1));

    model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0, 1, 0));
    model = glm::scale(model, glm::vec3(scale));

    GLint locModel = glGetUniformLocation(shaderProgram, "model");
    GLint locUseTex = glGetUniformLocation(shaderProgram, "useTexture");

    if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));

    if (textureID != 0)
    {
        if (locUseTex >= 0) glUniform1i(locUseTex, GL_TRUE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        GLint texLoc = glGetUniformLocation(shaderProgram, "texture1");
        if (texLoc >= 0) glUniform1i(texLoc, 0);
    }
    else
    {
        if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);
    }

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
    glBindVertexArray(0);
}

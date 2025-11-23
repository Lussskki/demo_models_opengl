#include "cube.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Cube::Cube(const glm::vec3& pos)
{
    position = pos;

    float cubeVertices[] = {
        -0.5f,-0.5f,-0.5f,        1,0,0,
         0.5f,-0.5f,-0.5f,        0,1,0,
         0.5f, 0.5f,-0.5f,        0,0,1,
         0.5f, 0.5f,-0.5f,        0,0,1,
        -0.5f, 0.5f,-0.5f,        1,1,0,
        -0.5f,-0.5f,-0.5f,        1,0,0,

        -0.5f,-0.5f, 0.5f,        1,0,1,
         0.5f,-0.5f, 0.5f,        0,1,1,
         0.5f, 0.5f, 0.5f,        1,1,1,
         0.5f, 0.5f, 0.5f,        1,1,1,
        -0.5f, 0.5f, 0.5f,        1,0,0,
        -0.5f,-0.5f, 0.5f,        1,0,1,

        -0.5f, 0.5f, 0.5f,        0,1,0,
        -0.5f, 0.5f,-0.5f,        0,0,1,
        -0.5f,-0.5f,-0.5f,        1,0,0,
        -0.5f,-0.5f,-0.5f,        1,0,0,
        -0.5f,-0.5f, 0.5f,        1,1,0,
        -0.5f, 0.5f, 0.5f,        0,1,0,

         0.5f, 0.5f, 0.5f,        1,0,1,
         0.5f, 0.5f,-0.5f,        0,1,1,
         0.5f,-0.5f,-0.5f,        0,0,1,
         0.5f,-0.5f,-0.5f,        0,0,1,
         0.5f,-0.5f, 0.5f,        1,1,1,
         0.5f, 0.5f, 0.5f,        1,0,1,

        -0.5f,-0.5f,-0.5f,        1,0,0,
         0.5f,-0.5f,-0.5f,        0,1,0,
         0.5f,-0.5f, 0.5f,        0,0,1,
         0.5f,-0.5f, 0.5f,        0,0,1,
        -0.5f,-0.5f, 0.5f,        1,1,0,
        -0.5f,-0.5f,-0.5f,        1,0,0,

        -0.5f, 0.5f,-0.5f,        0,1,1,
         0.5f, 0.5f,-0.5f,        1,0,1,
         0.5f, 0.5f, 0.5f,        0,1,0,
         0.5f, 0.5f, 0.5f,        0,1,0,
        -0.5f, 0.5f, 0.5f,        1,1,1,
        -0.5f, 0.5f,-0.5f,        0,1,1
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    GLsizei cubeStride = 6 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, cubeStride, (void*)0);
    glEnableVertexAttribArray(0);

    glDisableVertexAttribArray(1);
    glVertexAttrib3f(1, 0.0f, 0.0f, 1.0f);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, cubeStride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glDisableVertexAttribArray(3);
    glVertexAttrib2f(3, 0.0f, 0.0f);

    glBindVertexArray(0);
}

Cube::~Cube()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Cube::Draw(GLuint shaderProgram, GLint locModel, GLint locUseTex)
{
    glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), position);
    if (locModel >= 0) glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(modelCube));
    if (locUseTex >= 0) glUniform1i(locUseTex, GL_FALSE);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

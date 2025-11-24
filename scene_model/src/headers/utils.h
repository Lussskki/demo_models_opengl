#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "model.h"

// Function declarations
std::string readFile(const std::string& path);
GLuint loadTexture(const std::string& path);
bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture);
GLuint compileShader(GLenum type, const char* code);
GLuint createShaderProgram(const char* vertexCode, const char* fragmentCode);

#endif

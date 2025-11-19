#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>


#include "Camera.h"
#include "input_handler.h"

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 600;

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec2 TexCoords;
};

std::string readFile(const std::string& path);
GLuint loadTexture(const std::string& path);
bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture);
GLuint compileShader(GLenum type, const char* code);
GLuint createShaderProgram(const char* vertexCode, const char* fragmentCode);

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Models", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) { std::cerr << "GLEW init failed\n"; return -1; }

    glEnable(GL_DEPTH_TEST);

    Camera camera;
    InputHandler input(&camera, SCR_WIDTH, SCR_HEIGHT);
    glfwSetWindowUserPointer(window, &input);
    glfwSetCursorPosCallback(window, [](GLFWwindow* w, double x, double y) {
        static_cast<InputHandler*>(glfwGetWindowUserPointer(w))->MouseCallback(w, x, y);
        });
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Cube VAO/VBO setup 
    float cubeVertices[] = { 
        -0.5f,-0.5f,-0.5f, 1,0,0,  0.5f,-0.5f,-0.5f, 0,1,0,  0.5f,0.5f,-0.5f, 0,0,1,
         0.5f,0.5f,-0.5f,0,0,1, -0.5f,0.5f,-0.5f,1,1,0, -0.5f,-0.5f,-0.5f,1,0,0,
        -0.5f,-0.5f,0.5f,1,0,1, 0.5f,-0.5f,0.5f,0,1,1, 0.5f,0.5f,0.5f,1,1,1,
         0.5f,0.5f,0.5f,1,1,1, -0.5f,0.5f,0.5f,1,0,0, -0.5f,-0.5f,0.5f,1,0,1,
        -0.5f,0.5f,0.5f,0,1,0, -0.5f,0.5f,-0.5f,0,0,1, -0.5f,-0.5f,-0.5f,1,0,0,
        -0.5f,-0.5f,-0.5f,1,0,0, -0.5f,-0.5f,0.5f,1,1,0, -0.5f,0.5f,0.5f,0,1,0,
         0.5f,0.5f,0.5f,1,0,1, 0.5f,0.5f,-0.5f,0,1,1, 0.5f,-0.5f,-0.5f,0,0,1,
         0.5f,-0.5f,-0.5f,0,0,1, 0.5f,-0.5f,0.5f,1,1,1, 0.5f,0.5f,0.5f,1,0,1,
        -0.5f,-0.5f,-0.5f,1,0,0, 0.5f,-0.5f,-0.5f,0,1,0, 0.5f,-0.5f,0.5f,0,0,1,
         0.5f,-0.5f,0.5f,0,0,1, -0.5f,-0.5f,0.5f,1,1,0, -0.5f,-0.5f,-0.5f,1,0,0,
        -0.5f,0.5f,-0.5f,0,1,1, 0.5f,0.5f,-0.5f,1,0,1, 0.5f,0.5f,0.5f,0,1,0,
         0.5f,0.5f,0.5f,0,1,0, -0.5f,0.5f,0.5f,1,1,1, -0.5f,0.5f,-0.5f,0,1,1
    };
    GLuint VAO_cube, VBO_cube;
    glGenVertexArrays(1, &VAO_cube);
    glGenBuffers(1, &VBO_cube);
    glBindVertexArray(VAO_cube);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Shaders
    std::string vertexCode = readFile("shaders/shader.vert");
    std::string fragmentCode = readFile("shaders/shader.frag");
    GLuint shaderProgram = createShaderProgram(vertexCode.c_str(), fragmentCode.c_str());

    // Load Lion 
    std::vector<Vertex> qilinVertices;
    GLuint qilinTexture;
    if (!loadOBJ("qilin/qilin.obj", qilinVertices, qilinTexture))
        std::cerr << "Failed to load Qilin OBJ\n";

    GLuint VAO_qilin, VBO_qilin;
    glGenVertexArrays(1, &VAO_qilin);
    glGenBuffers(1, &VBO_qilin);
    glBindVertexArray(VAO_qilin);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_qilin);
    glBufferData(GL_ARRAY_BUFFER, qilinVertices.size() * sizeof(Vertex), qilinVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    //  Load Cottage
    std::vector<Vertex> cottageVertices;
    GLuint cottageTexture;
    if (!loadOBJ("cottage/cottage_obj.obj", cottageVertices, cottageTexture))
        std::cerr << "Failed to load Cottage OBJ\n";

    // Cottage VAO/VBO setup 
    GLuint VAO_cottage, VBO_cottage;
    glGenVertexArrays(1, &VAO_cottage);
    glGenBuffers(1, &VBO_cottage);
    glBindVertexArray(VAO_cottage);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cottage);
    glBufferData(GL_ARRAY_BUFFER, cottageVertices.size() * sizeof(Vertex), cottageVertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Positions & Scale
    glm::vec3 cubePos = glm::vec3(-1.0f, 0.0f, -5.0f);
    glm::vec3 qilinPos = glm::vec3(1.0f, 0.2f, -5.0f);
    float qilinScale = 0.0005f;

    glm::vec3 cottagePos = glm::vec3(4.0f, 0.0f, -8.0f);
    float cottageScale = 0.01f; 

    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        input.ProcessKeyboard(window, deltaTime);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        //  Draw Cube 
        glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), cubePos);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelCube));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), GL_FALSE);
        glBindVertexArray(VAO_cube);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        //  Draw Lion
        glm::mat4 modelQilin = glm::translate(glm::mat4(1.0f), qilinPos);

        modelQilin = glm::rotate(modelQilin, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelQilin = glm::rotate(modelQilin, glm::radians(360.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        modelQilin = glm::rotate(modelQilin, glm::radians(-300.0f), glm::vec3(0.0f, 0.0f, 1.0f));

        modelQilin = glm::scale(modelQilin, glm::vec3(qilinScale));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelQilin));
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), qilinTexture != 0);
        if (qilinTexture != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, qilinTexture);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        }

        glBindVertexArray(VAO_qilin);
        glDrawArrays(GL_TRIANGLES, 0, qilinVertices.size());
        glBindVertexArray(0);

        //  Draw Cottage
        glm::mat4 modelCottage = glm::translate(glm::mat4(1.0f), cottagePos);

        modelCottage = glm::scale(modelCottage, glm::vec3(cottageScale));

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelCottage));
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), cottageTexture != 0);
        if (cottageTexture != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cottageTexture);
            glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        }

        glBindVertexArray(VAO_cottage);
        glDrawArrays(GL_TRIANGLES, 0, cottageVertices.size());
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO_cube);
    glDeleteBuffers(1, &VBO_cube);
    glDeleteVertexArrays(1, &VAO_qilin);
    glDeleteBuffers(1, &VBO_qilin);
    glDeleteVertexArrays(1, &VAO_cottage);
    glDeleteBuffers(1, &VBO_cottage);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint loadTexture(const std::string& path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : (nrChannels == 3 ? GL_RGB : GL_RED);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
        textureID = 0;
    }
    return textureID;
}

bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string baseDir = path.substr(0, path.find_last_of("/\\") + 1);

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), baseDir.c_str(), true)) {
        std::cerr << "Failed to load OBJ: " << err << std::endl;
        return false;
    }
    if (!warn.empty())
        std::cout << "OBJ Warning: " << warn << std::endl;

    outTexture = 0;
    if (!materials.empty() && !materials[0].diffuse_texname.empty()) {
        std::string texturePath = baseDir + materials[0].diffuse_texname;
        outTexture = loadTexture(texturePath);
        if (outTexture == 0) std::cout << "Texture specified as '" << materials[0].diffuse_texname << "' not found (full path: " << texturePath << "), using vertex colors\n";
    }

    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            int matID = (shape.mesh.material_ids.size() > f) ? shape.mesh.material_ids[f] : -1;
            glm::vec3 faceColor(0.8f);
            if (matID >= 0 && matID < materials.size())
                faceColor = glm::vec3(materials[matID].diffuse[0], materials[matID].diffuse[1], materials[matID].diffuse[2]);

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                Vertex vertex;
                vertex.Position = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                };
                vertex.Color = faceColor;
                vertex.TexCoords = (idx.texcoord_index >= 0) ?
                    glm::vec2(attrib.texcoords[2 * idx.texcoord_index], attrib.texcoords[2 * idx.texcoord_index + 1]) :
                    glm::vec2(0.0f, 0.0f);
                outVertices.push_back(vertex);
            }
            index_offset += fv;
        }
    }
    std::cout << "Loaded OBJ: " << path << " (" << outVertices.size() << " vertices)\n";
    return true;
}

GLuint compileShader(GLenum type, const char* code) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compile error: " << infoLog << std::endl;
    } 
    return shader;
}

GLuint createShaderProgram(const char* vertexCode, const char* fragmentCode) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    int success;
    char infoLog[512]; 
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader link error: " << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}
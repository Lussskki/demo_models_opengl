#include <iostream>
#include <fstream>
#include <sstream>

#include "tiny_obj_loader.h" 
#include "headers/stb_image.h" 
#include "utils.h"


std::string readFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << path << "\n";
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint loadTexture(const std::string& path)
{
    GLuint textureID = 0;
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
        if (data) stbi_image_free(data);
        textureID = 0;
    }
    return textureID;
}

bool loadOBJ(const std::string& path, std::vector<Vertex>& outVertices, GLuint& outTexture)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    std::string baseDir = path.substr(0, path.find_last_of("/\\") + 1);

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str(), baseDir.c_str(), true);
    if (!ret)
    {
        std::cerr << "Failed to load OBJ: " << err << std::endl;
        return false;
    }
    if (!warn.empty()) std::cout << "OBJ Warning: " << warn << std::endl;

    outTexture = 0;
    if (!materials.empty() && !materials[0].diffuse_texname.empty())
    {
        std::string texturePath = baseDir + materials[0].diffuse_texname;
        outTexture = loadTexture(texturePath);
        if (outTexture == 0) std::cout << "Texture '" << materials[0].diffuse_texname << "' not found, using vertex colors\n";
    }

    std::cout << "Loaded OBJ: " << path << " (" << attrib.vertices.size() / 3 << " vertices)\n";


    bool hasNormals = !attrib.normals.empty();
    for (const auto& shape : shapes)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
        {
            int fv = shape.mesh.num_face_vertices[f];
            int matID = (shape.mesh.material_ids.size() > f) ? shape.mesh.material_ids[f] : -1;
            glm::vec3 faceColor(0.8f, 0.8f, 0.8f);
            if (matID >= 0 && matID < (int)materials.size()) {
                faceColor = glm::vec3(materials[matID].diffuse[0], materials[matID].diffuse[1], materials[matID].diffuse[2]);
            }

            glm::vec3 computedNormal(0.0f);
            if (!hasNormals && fv >= 3)
            {
                tinyobj::index_t idx0 = shape.mesh.indices[index_offset + 0];
                tinyobj::index_t idx1 = shape.mesh.indices[index_offset + 1];
                tinyobj::index_t idx2 = shape.mesh.indices[index_offset + 2];
                glm::vec3 v0(
                    attrib.vertices[3 * idx0.vertex_index + 0],
                    attrib.vertices[3 * idx0.vertex_index + 1],
                    attrib.vertices[3 * idx0.vertex_index + 2]
                );
                glm::vec3 v1(
                    attrib.vertices[3 * idx1.vertex_index + 0],
                    attrib.vertices[3 * idx1.vertex_index + 1],
                    attrib.vertices[3 * idx1.vertex_index + 2]
                );
                glm::vec3 v2(
                    attrib.vertices[3 * idx2.vertex_index + 0],
                    attrib.vertices[3 * idx2.vertex_index + 1],
                    attrib.vertices[3 * idx2.vertex_index + 2]
                );
                computedNormal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            }

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                Vertex vertex;
                vertex.Position = {
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                };

                if (idx.normal_index >= 0 && hasNormals)
                {
                    vertex.Normal = {
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    };
                }
                else {
                    vertex.Normal = computedNormal;
                }

                vertex.Color = faceColor;

                if (idx.texcoord_index >= 0 && !attrib.texcoords.empty())
                {
                    vertex.TexCoords = {
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    };
                }
                else {
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);
                }

                outVertices.push_back(vertex);
            }
            index_offset += fv;
        }
    }

    return true;
}

GLuint compileShader(GLenum type, const char* code) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    int success;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
        std::cerr << "Shader compile error: " << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexCode, const char* fragmentCode) {
    GLuint vertex = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    int success;
    char infoLog[1024];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 1024, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}
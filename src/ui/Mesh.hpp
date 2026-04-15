#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "GLHeaders.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh {
public:
    Mesh() : m_VAO(0), m_VBO(0), m_EBO(0), m_indexCount(0), m_textureID(0) {}
    ~Mesh();

    Mesh(const Mesh&)            = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&&) noexcept;
    Mesh& operator=(Mesh&&) noexcept;

    // Upload mesh data to GPU
    void uploadData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // Upload texture data
    void setTexture(GLuint textureID) { m_textureID = textureID; }

    // Render the mesh
    void render() const;

    // Getters
    GLuint getVAO() const { return m_VAO; }
    GLuint getVBO() const { return m_VBO; }
    GLuint getEBO() const { return m_EBO; }
    unsigned int getIndexCount() const { return m_indexCount; }
    GLuint getTextureID() const { return m_textureID; }

private:
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    unsigned int m_indexCount;
    GLuint m_textureID;

    void cleanup();
};

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include "glimac/Program.hpp"
#include "Mesh.hpp"

class Skybox {
public:
    Skybox();
    ~Skybox();
    
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    Skybox(Skybox&&) = delete;
    Skybox& operator=(Skybox&&) = delete;

    // Initialize skybox - either with procedural or from glb model
    bool initialize(const std::string& executablePath = "", bool useModelledSkybox = false);

    // Load skybox model from .glb file
    bool loadFromGLB(const std::string& filePath);

    // Render the skybox
    void render(const glm::mat4& viewProjectionMatrix);

private:
    GLuint m_VAO;
    GLuint m_VBO;
    GLuint m_EBO;
    std::unique_ptr<glimac::Program> m_program;
    std::unique_ptr<Mesh> m_skyboxMesh;  // For .glb models
    bool m_useModel;
    unsigned int m_indexCount;
    
    // Create cube geometry for skybox (procedural)
    void createCubeGeometry();
};

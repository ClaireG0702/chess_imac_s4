#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include "glimac/Program.hpp"

class Skybox {
public:
    Skybox();
    ~Skybox();
    
    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;
    Skybox(Skybox&&) = delete;
    Skybox& operator=(Skybox&&) = delete;

    // Initialize skybox with procedural shader
    bool initialize(const std::string& executablePath = "");

    // Render the skybox
    void render(const glm::mat4& viewProjectionMatrix);

private:
    GLuint m_VAO;
    GLuint m_VBO;
    std::unique_ptr<glimac::Program> m_program;
    
    // Create cube geometry for skybox
    void createCubeGeometry();
};

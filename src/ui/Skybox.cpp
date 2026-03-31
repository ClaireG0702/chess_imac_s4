#include "GLHeaders.hpp"
#include "Skybox.hpp"
#include "glimac/FilePath.hpp"
#include <iostream>

Skybox::Skybox()
    : m_VAO(0), m_VBO(0) {
}

Skybox::~Skybox() {
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
}

bool Skybox::initialize(const std::string& executablePath) {
    try {
        // Load shaders using executable path
        glimac::FilePath applicationPath(executablePath);
        glimac::FilePath skyboxVs(applicationPath.dirPath() + "../../assets/shaders/skybox.vs.glsl");
        glimac::FilePath skyboxFs(applicationPath.dirPath() + "../../assets/shaders/skybox.fs.glsl");
        
        m_program = std::make_unique<glimac::Program>(
            glimac::loadProgram(skyboxVs, skyboxFs)
        );
    } catch (const std::exception& e) {
        std::cerr << "Skybox shader loading error: " << e.what() << std::endl;
        return false;
    }

    // Create cube geometry
    createCubeGeometry();

    return true;
}

void Skybox::createCubeGeometry() {
    // Cube vertices (skybox cube, large size)
    float size = 50.0f;
    float vertices[] = {
        // Back face
        -size, -size, -size,
         size, -size, -size,
         size,  size, -size,
        -size,  size, -size,
        // Front face
        -size, -size,  size,
         size, -size,  size,
         size,  size,  size,
        -size,  size,  size,
        // Left face
        -size, -size, -size,
        -size,  size, -size,
        -size,  size,  size,
        -size, -size,  size,
        // Right face
         size, -size, -size,
         size,  size, -size,
         size,  size,  size,
         size, -size,  size,
        // Bottom face
        -size, -size, -size,
        -size, -size,  size,
         size, -size,  size,
         size, -size, -size,
        // Top face
        -size,  size, -size,
         size,  size, -size,
         size,  size,  size,
        -size,  size,  size,
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Skybox::render(const glm::mat4& viewProjectionMatrix) {
    glDepthFunc(GL_LEQUAL);  // Change depth function so skybox is drawn behind everything
    
    m_program->use();
    
    // Set uniform for view-projection (without translation for infinite skybox effect)
    glm::mat4 skyboxViewProj = viewProjectionMatrix;
    GLint vpLoc = glGetUniformLocation(m_program->getGLId(), "viewProjectionMatrix");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &skyboxViewProj[0][0]);

    // Time uniform for animation
    static float timeCounter = 0.0f;
    timeCounter += 0.016f; // Approximate 60 FPS
    GLint timeLoc = glGetUniformLocation(m_program->getGLId(), "time");
    glUniform1f(timeLoc, timeCounter);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_QUADS, 0, 24);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);  // Restore default depth function
}

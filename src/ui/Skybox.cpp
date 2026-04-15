#include "GLHeaders.hpp"
#include "Skybox.hpp"
#include "GLTFLoader.hpp"
#include "glimac/FilePath.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

Skybox::Skybox()
    : m_VAO(0), m_VBO(0), m_EBO(0), m_useModel(false), m_indexCount(0) {
}

Skybox::~Skybox() {
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
    if (m_EBO != 0) glDeleteBuffers(1, &m_EBO);
}

bool Skybox::initialize(const std::string& executablePath, bool useModelledSkybox) {
    m_useModel = useModelledSkybox;

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

    if (!useModelledSkybox) {
        // Create procedural cube geometry
        createCubeGeometry();
    } else {
        // Load from GLB model
        glimac::FilePath applicationPath(executablePath);
        std::string modelPath = applicationPath.dirPath() + "../../assets/models/skybox/skybox.glb";
        if (!loadFromGLB(modelPath)) {
            std::cerr << "Failed to load skybox model, falling back to procedural cube" << std::endl;
            m_useModel = false;
            createCubeGeometry();
        }
    }

    return true;
}

bool Skybox::loadFromGLB(const std::string& filePath) {
    try {
        std::vector<Mesh> meshes = GLTFLoader::loadGLB(filePath);
        if (meshes.empty()) {
            std::cerr << "No meshes loaded from skybox model: " << filePath << std::endl;
            return false;
        }
        // Take the first mesh as the skybox mesh (move it to unique_ptr)
        m_skyboxMesh = std::make_unique<Mesh>(std::move(meshes[0]));
        std::cout << "Skybox model loaded successfully from: " << filePath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading skybox model: " << e.what() << std::endl;
        return false;
    }
}

void Skybox::createCubeGeometry() {
    // Cube vertices - HUGE size to cover entire view
    float size = 1000.0f;  // Massively increased
    float vertices[] = {
        // Front face
        -size, -size,  size,
         size, -size,  size,
         size,  size,  size,
        -size,  size,  size,
        // Back face
        -size, -size, -size,
        -size,  size, -size,
         size,  size, -size,
         size, -size, -size,
        // Left face
        -size, -size, -size,
        -size, -size,  size,
        -size,  size,  size,
        -size,  size, -size,
        // Right face
         size, -size, -size,
         size,  size, -size,
         size,  size,  size,
         size, -size,  size,
        // Top face
        -size,  size, -size,
        -size,  size,  size,
         size,  size,  size,
         size,  size, -size,
        // Bottom face
        -size, -size, -size,
         size, -size, -size,
         size, -size,  size,
        -size, -size,  size,
    };

    // Indices for triangles (each face is 2 triangles = 6 indices)
    unsigned int indices[] = {
        // Front face
        0, 1, 2,
        0, 2, 3,
        // Back face
        4, 6, 5,
        4, 7, 6,
        // Left face
        8, 9, 10,
        8, 10, 11,
        // Right face
        12, 13, 14,
        12, 14, 15,
        // Top face
        16, 17, 18,
        16, 18, 19,
        // Bottom face
        20, 21, 22,
        20, 22, 23,
    };

    m_indexCount = 36;  // 6 faces * 6 indices per face

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Skybox::render(const glm::mat4& viewProjectionMatrix) {
    // Save current culling state
    GLboolean cullFaceEnabled = glIsEnabled(GL_CULL_FACE);
    
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);  // Don't write to depth buffer - skybox stays behind
    glDisable(GL_CULL_FACE);
    
    m_program->use();
    
    GLint vpLoc = glGetUniformLocation(m_program->getGLId(), "viewProjectionMatrix");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &viewProjectionMatrix[0][0]);
    
    GLint modelLoc = glGetUniformLocation(m_program->getGLId(), "modelMatrix");
    
    // Set texture sampler uniform
    GLint samplerLoc = glGetUniformLocation(m_program->getGLId(), "albedoTexture");
    glUniform1i(samplerLoc, 0);  // Texture slot 0

    if (m_useModel && m_skyboxMesh) {
        // Scale skybox bigger to fill background + rotate for proper orientation
        glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 scaleMatrix = glm::scale(rotateMatrix, glm::vec3(50.0f, 50.0f, 50.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &scaleMatrix[0][0]);
        
        m_skyboxMesh->render();
    } else {
        glm::mat4 identityMatrix = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &identityMatrix[0][0]);
        
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    glDepthMask(GL_TRUE);   // Re-enable depth writing
    glDepthFunc(GL_LESS);
    
    // Restore culling state
    if (cullFaceEnabled)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

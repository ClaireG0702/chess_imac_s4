#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include "Camera3D.hpp"
#include "Skybox.hpp"
#include "glimac/Program.hpp"
#include "glimac/glimac/Program.hpp"

class GameState;

class Renderer3D {
public:
    Renderer3D();
    ~Renderer3D();
    
    Renderer3D(const Renderer3D&) = delete;
    Renderer3D& operator=(const Renderer3D&) = delete;
    Renderer3D(Renderer3D&&) = delete;
    Renderer3D& operator=(Renderer3D&&) = delete;

    // Initialize 3D renderer (shaders, geometry, framebuffer)
    bool initialize(int width, int height);

    // Render the 3D chess scene to framebuffer
    void render(const GameState& gameState);

    // Get the framebuffer texture ID for ImGui display
    GLuint getFramebufferTexture() const { return m_framebufferTexture; }

    // Update viewport size
    void setViewportSize(int width, int height);

private:
    // Framebuffer objects
    GLuint m_framebuffer;
    GLuint m_framebufferTexture;
    GLuint m_depthRenderbuffer;
    int m_framebufferWidth;
    int m_framebufferHeight;

    // Camera and skybox
    Camera3D m_camera;
    std::unique_ptr<Skybox> m_skybox;

    // Shader programs
    std::unique_ptr<glimac::Program> m_boardProgram;
    std::unique_ptr<glimac::Program> m_pieceProgram;

    // Board geometry
    GLuint m_boardVAO, m_boardVBO, m_boardEBO;
    unsigned int m_boardIndexCount;

    // Piece geometry (reusable cube for all pieces)
    GLuint m_pieceVAO, m_pieceVBO, m_pieceEBO;
    unsigned int m_pieceIndexCount;

    // Initialization helpers
    bool createFramebuffer(int width, int height);
    bool createShaders();
    bool createBoardGeometry();
    bool createPieceGeometry();
    void deletePlaneGeometry();
    
    // Rendering helpers
    void drawBoard(const GameState& gameState);
    void drawPieces(const GameState& gameState);
    void drawSelectedHighlight(const GameState& gameState);
    void drawPossibleMovesHighlight(const GameState& gameState);
};

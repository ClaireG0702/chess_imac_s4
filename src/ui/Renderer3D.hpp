#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <string>
#include "Skybox.hpp"
#include "glimac/Program.hpp"
#include "glimac/glimac/Program.hpp"
#include "glimac/glimac/TrackballCamera.hpp"
#include "glimac/glimac/FreeflyCamera.hpp"

class GameState;

enum class CameraMode {
    Trackball,
    Freefly
};

class Renderer3D {
public:
    Renderer3D();
    ~Renderer3D();
    
    Renderer3D(const Renderer3D&) = delete;
    Renderer3D& operator=(const Renderer3D&) = delete;
    Renderer3D(Renderer3D&&) = delete;
    Renderer3D& operator=(Renderer3D&&) = delete;

    // Initialize 3D renderer (shaders, geometry, framebuffer)
    bool initialize(const std::string& executablePath, int width, int height);

    // Render the 3D chess scene to framebuffer
    void render(const GameState& gameState);

    // Get the framebuffer texture ID for ImGui display
    GLuint getFramebufferTexture() const { return m_framebufferTexture; }

    // Update viewport size
    void setViewportSize(int width, int height);
    
    // Camera management
    void setCameraMode(CameraMode mode);
    CameraMode getCameraMode() const { return m_cameraMode; }
    
    // Trackball camera controls
    void rotateTrackballLeft(float degrees);
    void rotateTrackballUp(float degrees);
    void zoomTrackball(float delta);

    // Freefly camera controls
    void moveFreeflyFront(float t);
    void moveFreeflyLeft(float t);
    void moveFreeflyUp(float t);
    void rotateFreeflyLeft(float degrees);
    void rotateFreeflyUp(float degrees);

private:
    // Executable path for asset loading
    std::string m_executablePath;

    // Framebuffer objects
    GLuint m_framebuffer;
    GLuint m_framebufferTexture;
    GLuint m_depthRenderbuffer;
    int m_framebufferWidth;
    int m_framebufferHeight;

    // Camera management
    CameraMode m_cameraMode;
    glimac::TrackballCamera m_trackballCamera;
    glimac::FreeflyCamera m_freeflyCamera;
    
    // Skybox
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
    glm::mat4 getViewProjectionMatrix() const;
    void drawBoard(const GameState& gameState);
    void drawPieces(const GameState& gameState);
    void drawSelectedHighlight(const GameState& gameState);
    void drawPossibleMovesHighlight(const GameState& gameState);
};

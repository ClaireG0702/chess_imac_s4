#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include "Skybox.hpp"
#include "PieceModel.hpp"
#include "PieceAnimation.hpp"
#include "SelectionState.hpp"
#include "glimac/glimac/Program.hpp"
#include "glimac/glimac/TrackballCamera.hpp"
#include "glimac/glimac/PieceCamera.hpp"

class GameState;

enum class CameraMode {
    Trackball,
    Piece
};

class Renderer3D {
public:
    Renderer3D();
    ~Renderer3D();

    Renderer3D(const Renderer3D&)            = delete;
    Renderer3D& operator=(const Renderer3D&) = delete;
    Renderer3D(Renderer3D&&)                 = delete;
    Renderer3D& operator=(Renderer3D&&)      = delete;

    // Initialize 3D renderer (shaders, geometry, framebuffer)
    bool initialize(const std::string& executablePath, int width, int height);

    // Render the 3D chess scene to framebuffer
    void render(const GameState& gameState);

    // Get the framebuffer texture ID for ImGui display
    GLuint getFramebufferTexture() const { return m_framebufferTexture; }

    // Update viewport size
    void setViewportSize(int width, int height);

    // Camera management
    void       setCameraMode(CameraMode mode);
    CameraMode getCameraMode() const { return m_cameraMode; }

    // Trackball camera controls
    void rotateTrackballLeft(float degrees);
    void rotateTrackballUp(float degrees);
    void zoomTrackball(float delta);

    // Piece camera controls
    void rotatePieceLeft(float degrees);
    void rotatePieceUp(float degrees);

    // Animation controls
    void animatePieceMovement(int fromRow, int fromCol, int toRow, int toCol, float duration = 0.5f);
    void updateAnimation();

    // Ray-casting and cell selection
    std::pair<int, int> getCellFromMousePosition(float screenX, float screenY);
    void                updateHoverableCell(float screenX, float screenY);
    void                updateCellStates(const GameState& gameState);

private:
    // Executable path for asset loading
    std::string m_executablePath;

    // Framebuffer objects
    GLuint m_framebuffer;
    GLuint m_framebufferTexture;
    GLuint m_depthRenderbuffer;
    int    m_framebufferWidth;
    int    m_framebufferHeight;

    // Camera management
    CameraMode              m_cameraMode;
    glimac::TrackballCamera m_trackballCamera;
    glimac::PieceCamera     m_pieceCamera;

    // Piece movement animation
    PieceAnimationManager m_pieceAnimationManager;

    // Skybox
    std::unique_ptr<Skybox> m_skybox;

    // Piece models loaded from glTF/glb files
    std::unique_ptr<PieceModel> m_pieceModels;
    bool                        m_useModeledPieces; // Flag to use 3D models vs simple cubes

    // Shader programs
    std::unique_ptr<glimac::Program> m_boardProgram;
    std::unique_ptr<glimac::Program> m_pieceProgram;

    // Board geometry
    GLuint       m_boardVAO, m_boardVBO, m_boardEBO;
    GLuint       m_boardColorVBO;  // Color buffer for state-based coloring
    unsigned int m_boardIndexCount;

    // Piece geometry (reusable cube for all pieces)
    GLuint       m_pieceVAO, m_pieceVBO, m_pieceEBO;
    unsigned int m_pieceIndexCount;

    // Board cell states for selection highlighting
    std::vector<std::vector<CellState>> m_cellStates;  // 8x8 grid of cell states
    int                                  m_lastHoveredRow;
    int                                  m_lastHoveredCol;

    // Board geometry constants
    static constexpr float BOARD_START_X = 0.0f;
    static constexpr float BOARD_START_Z = 0.0f;
    static constexpr float SQUARE_SIZE = 1.0f;
    static constexpr float BOARD_HEIGHT = 0.4f;

    // Initialization helpers
    bool createFramebuffer(int width, int height);
    bool createShaders();
    bool createBoardGeometry();
    bool createPieceGeometry();
    void deletePlaneGeometry();

    // Rendering helpers
    glm::mat4 getViewProjectionMatrix() const;
    void      drawBoard(const GameState& gameState);
    void      drawPieces(const GameState& gameState);
    void      drawSelectedHighlight(const GameState& gameState);
    void      drawPossibleMovesHighlight(const GameState& gameState);

    // Ray-casting helpers
    std::pair<glm::vec3, glm::vec3> getRayOriginAndDirection(float screenX, float screenY) const;
    bool      rayCastSquare(const glm::vec3& rayOrigin, const glm::vec3& rayDir, int row, int col, glm::vec3& intersection) const;
    glm::vec3 getColorForCellState(CellSelectionState state, bool isLightSquare) const;

    bool m_daltonismMode = false;
};

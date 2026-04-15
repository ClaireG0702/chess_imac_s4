#include "Renderer3D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include "GLHeaders.hpp"
#include "core/Board.hpp"
#include "core/GameState.hpp"
#include "core/Piece.hpp"
#include "glimac/FilePath.hpp"

Renderer3D::Renderer3D()
    : m_framebuffer(0), m_framebufferTexture(0), m_depthRenderbuffer(0), m_framebufferWidth(800), m_framebufferHeight(800), m_cameraMode(CameraMode::Trackball), m_trackballCamera(15.0f, 0.0f, 30.0f), m_boardVAO(0), m_boardVBO(0), m_boardEBO(0), m_boardIndexCount(0), m_pieceVAO(0), m_pieceVBO(0), m_pieceEBO(0), m_pieceIndexCount(0), m_useModeledPieces(false)
{
}

Renderer3D::~Renderer3D()
{
    deletePlaneGeometry();

    if (m_boardVAO != 0)
        glDeleteVertexArrays(1, &m_boardVAO);
    if (m_boardVBO != 0)
        glDeleteBuffers(1, &m_boardVBO);
    if (m_boardEBO != 0)
        glDeleteBuffers(1, &m_boardEBO);

    if (m_pieceVAO != 0)
        glDeleteVertexArrays(1, &m_pieceVAO);
    if (m_pieceVBO != 0)
        glDeleteBuffers(1, &m_pieceVBO);
    if (m_pieceEBO != 0)
        glDeleteBuffers(1, &m_pieceEBO);

    if (m_framebuffer != 0)
        glDeleteFramebuffers(1, &m_framebuffer);
    if (m_framebufferTexture != 0)
        glDeleteTextures(1, &m_framebufferTexture);
    if (m_depthRenderbuffer != 0)
        glDeleteRenderbuffers(1, &m_depthRenderbuffer);
}

bool Renderer3D::initialize(const std::string& executablePath, int width, int height)
{
    m_executablePath    = executablePath;
    m_framebufferWidth  = width;
    m_framebufferHeight = height;

    if (!createFramebuffer(width, height))
    {
        std::cerr << "Failed to create framebuffer" << std::endl;
        return false;
    }

    if (!createShaders())
    {
        std::cerr << "Failed to create shaders" << std::endl;
        return false;
    }

    if (!createBoardGeometry())
    {
        std::cerr << "Failed to create board geometry" << std::endl;
        return false;
    }

    if (!createPieceGeometry())
    {
        std::cerr << "Failed to create piece geometry" << std::endl;
        return false;
    }

    // Initialize skybox with GLB model
    m_skybox = std::make_unique<Skybox>();
    if (!m_skybox->initialize(m_executablePath, true))
    {
        std::cerr << "Failed to initialize skybox" << std::endl;
        return false;
    }

    // Try to load piece models from glTF/glb files
    glimac::FilePath applicationPath(m_executablePath);
    std::string modelsPath = applicationPath.dirPath() + "../../assets/models";
    
    m_pieceModels = std::make_unique<PieceModel>();
    if (m_pieceModels->loadFromDirectory(modelsPath))
    {
        m_useModeledPieces = true;
        std::cout << "Successfully loaded piece models from: " << modelsPath << std::endl;
    }
    else
    {
        m_useModeledPieces = false;
        std::cout << "Piece model directory not found or files missing. Using fallback cube geometry." << std::endl;
        std::cout << "Models directory path: " << modelsPath << std::endl;
        std::cout << "Place your .glb files at: " << modelsPath << "/pawn.glb, knight.glb, etc." << std::endl;
    }

    return true;
}

bool Renderer3D::createFramebuffer(int width, int height)
{
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    // Create texture for color attachment
    glGenTextures(1, &m_framebufferTexture);
    glBindTexture(GL_TEXTURE_2D, m_framebufferTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_framebufferTexture, 0);

    // Create renderbuffer for depth attachment
    glGenRenderbuffers(1, &m_depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderbuffer);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer is not complete!" << std::endl;
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

bool Renderer3D::createShaders()
{
    try
    {
        // Load shaders using executable path, just like the example:
        // glimac::FilePath applicationPath(argv[0]);
        // glimac::Program program = loadProgram(
        //     applicationPath.dirPath() + "TP4/shaders/3D.vs.glsl",
        //     applicationPath.dirPath() + "TP4/shaders/tex3D.fs.glsl"
        // );
        glimac::FilePath applicationPath(m_executablePath);
        glimac::FilePath boardVs(applicationPath.dirPath() + "../../assets/shaders/board.vs.glsl");
        glimac::FilePath boardFs(applicationPath.dirPath() + "../../assets/shaders/board.fs.glsl");
        glimac::FilePath pieceVs(applicationPath.dirPath() + "../../assets/shaders/blinnphong.vs.glsl");
        glimac::FilePath pieceFs(applicationPath.dirPath() + "../../assets/shaders/blinnphong.fs.glsl");

        m_boardProgram = std::make_unique<glimac::Program>(
            glimac::loadProgram(boardVs, boardFs)
        );
        if (!m_boardProgram)
        {
            std::cerr << "Board program is null" << std::endl;
            return false;
        }

        m_pieceProgram = std::make_unique<glimac::Program>(
            glimac::loadProgram(pieceVs, pieceFs)
        );
        if (!m_pieceProgram)
        {
            std::cerr << "Piece program is null" << std::endl;
            return false;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Shader compilation error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool Renderer3D::createBoardGeometry()
{
    // Create 8x8 board with quads (light/dark squares) + simple border
    std::vector<float>        vertices;
    std::vector<float>        colors;
    std::vector<unsigned int> indices;

    float squareSize   = 1.0f;
    float boardStartX  = 0.0f;
    float boardStartZ  = 0.0f;
    float boardHeight  = 0.4f;   // Thicker board surface
    float baseHeight   = 0.0f;   // Bottom of base
    float borderWidth  = 0.3f;   // Width of border around board

    unsigned int vertexCount = 0;

    // Border gray color
    float bgr = 100.0f / 255.0f, bgg = 100.0f / 255.0f, bgb = 100.0f / 255.0f;

    // ===== CREATE SIMPLE FLAT BASE PLATFORM =====
    float baseLeft   = boardStartX - borderWidth;
    float baseRight  = boardStartX + 8.0f * squareSize + borderWidth;
    float baseTop    = boardStartZ - borderWidth;
    float baseBottom = boardStartZ + 8.0f * squareSize + borderWidth;

    // Simple flat rectangular base
    vertices.push_back(baseLeft);
    vertices.push_back(baseHeight);
    vertices.push_back(baseTop);
    colors.push_back(bgr);
    colors.push_back(bgg);
    colors.push_back(bgb);

    vertices.push_back(baseRight);
    vertices.push_back(baseHeight);
    vertices.push_back(baseTop);
    colors.push_back(bgr);
    colors.push_back(bgg);
    colors.push_back(bgb);

    vertices.push_back(baseRight);
    vertices.push_back(baseHeight);
    vertices.push_back(baseBottom);
    colors.push_back(bgr);
    colors.push_back(bgg);
    colors.push_back(bgb);

    vertices.push_back(baseLeft);
    vertices.push_back(baseHeight);
    vertices.push_back(baseBottom);
    colors.push_back(bgr);
    colors.push_back(bgg);
    colors.push_back(bgb);

    // Base quad indices (2 triangles for top face)
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(0);
    indices.push_back(2);
    indices.push_back(3);

    vertexCount = 4;

    // ===== ADD SIDE FACES FOR THICKNESS =====
    // Front face (baseTop edge)
    vertices.push_back(baseLeft);   // 4
    vertices.push_back(baseHeight);
    vertices.push_back(baseTop);
    colors.push_back(bgr * 0.8f);
    colors.push_back(bgg * 0.8f);
    colors.push_back(bgb * 0.8f);

    vertices.push_back(baseRight);  // 5
    vertices.push_back(baseHeight);
    vertices.push_back(baseTop);
    colors.push_back(bgr * 0.8f);
    colors.push_back(bgg * 0.8f);
    colors.push_back(bgb * 0.8f);

    vertices.push_back(baseRight);  // 6
    vertices.push_back(baseHeight - 0.5f);  // Bottom
    vertices.push_back(baseTop);
    colors.push_back(bgr * 0.6f);
    colors.push_back(bgg * 0.6f);
    colors.push_back(bgb * 0.6f);

    vertices.push_back(baseLeft);   // 7
    vertices.push_back(baseHeight - 0.5f);  // Bottom
    vertices.push_back(baseTop);
    colors.push_back(bgr * 0.6f);
    colors.push_back(bgg * 0.6f);
    colors.push_back(bgb * 0.6f);

    // Front face indices
    indices.push_back(4);
    indices.push_back(5);
    indices.push_back(6);
    indices.push_back(4);
    indices.push_back(6);
    indices.push_back(7);
    vertexCount += 4;

    // Back face (baseBottom edge)
    vertices.push_back(baseRight);  // 8
    vertices.push_back(baseHeight);
    vertices.push_back(baseBottom);
    colors.push_back(bgr * 0.8f);
    colors.push_back(bgg * 0.8f);
    colors.push_back(bgb * 0.8f);

    vertices.push_back(baseLeft);   // 9
    vertices.push_back(baseHeight);
    vertices.push_back(baseBottom);
    colors.push_back(bgr * 0.8f);
    colors.push_back(bgg * 0.8f);
    colors.push_back(bgb * 0.8f);

    vertices.push_back(baseLeft);   // 10
    vertices.push_back(baseHeight - 0.5f);
    vertices.push_back(baseBottom);
    colors.push_back(bgr * 0.6f);
    colors.push_back(bgg * 0.6f);
    colors.push_back(bgb * 0.6f);

    vertices.push_back(baseRight);  // 11
    vertices.push_back(baseHeight - 0.5f);
    vertices.push_back(baseBottom);
    colors.push_back(bgr * 0.6f);
    colors.push_back(bgg * 0.6f);
    colors.push_back(bgb * 0.6f);

    // Back face indices
    indices.push_back(8);
    indices.push_back(9);
    indices.push_back(10);
    indices.push_back(8);
    indices.push_back(10);
    indices.push_back(11);
    vertexCount += 4;

    // Left face (baseLeft edge)
    vertices.push_back(baseLeft);   // 12
    vertices.push_back(baseHeight);
    vertices.push_back(baseBottom);
    colors.push_back(bgr * 0.8f);
    colors.push_back(bgg * 0.8f);
    colors.push_back(bgb * 0.8f);

    vertices.push_back(baseLeft);   // 13
    vertices.push_back(baseHeight);
    vertices.push_back(baseTop);
    colors.push_back(bgr * 0.8f);
    colors.push_back(bgg * 0.8f);
    colors.push_back(bgb * 0.8f);

    vertices.push_back(baseLeft);   // 14
    vertices.push_back(baseHeight - 0.5f);
    vertices.push_back(baseTop);
    colors.push_back(bgr * 0.6f);
    colors.push_back(bgg * 0.6f);
    colors.push_back(bgb * 0.6f);

    vertices.push_back(baseLeft);   // 15
    vertices.push_back(baseHeight - 0.5f);
    vertices.push_back(baseBottom);
    colors.push_back(bgr * 0.6f);
    colors.push_back(bgg * 0.6f);
    colors.push_back(bgb * 0.6f);

    // Left face indices
    indices.push_back(12);
    indices.push_back(13);
    indices.push_back(14);
    indices.push_back(12);
    indices.push_back(14);
    indices.push_back(15);
    vertexCount += 4;

    // Right face (baseRight edge)
    vertices.push_back(baseRight);  // 16
    vertices.push_back(baseHeight);
    vertices.push_back(baseTop);
    colors.push_back(bgr * 0.8f);
    colors.push_back(bgg * 0.8f);
    colors.push_back(bgb * 0.8f);

    vertices.push_back(baseRight);  // 17
    vertices.push_back(baseHeight);
    vertices.push_back(baseBottom);
    colors.push_back(bgr * 0.8f);
    colors.push_back(bgg * 0.8f);
    colors.push_back(bgb * 0.8f);

    vertices.push_back(baseRight);  // 18
    vertices.push_back(baseHeight - 0.5f);
    vertices.push_back(baseBottom);
    colors.push_back(bgr * 0.6f);
    colors.push_back(bgg * 0.6f);
    colors.push_back(bgb * 0.6f);

    vertices.push_back(baseRight);  // 19
    vertices.push_back(baseHeight - 0.5f);
    vertices.push_back(baseTop);
    colors.push_back(bgr * 0.6f);
    colors.push_back(bgg * 0.6f);
    colors.push_back(bgb * 0.6f);

    // Right face indices
    indices.push_back(16);
    indices.push_back(17);
    indices.push_back(18);
    indices.push_back(16);
    indices.push_back(18);
    indices.push_back(19);
    vertexCount += 4;

    // ===== CREATE BOARD SQUARES =====
    // Generate 64 squares (8x8)
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            float x = boardStartX + col * squareSize;
            float z = boardStartZ + (7 - row) * squareSize;
            float y = baseHeight + 0.001f;  // Just above the base to avoid z-fighting

            // Determine color: light (white-ish) or dark (blue-ish)
            bool  isLight = ((row + col) % 2) != 0;
            float r, g, b;
            if (isLight)
            {
                // Light cyan-ish
                r = 173.0f / 255.0f;
                g = 232.0f / 255.0f;
                b = 244.0f / 255.0f;
            }
            else
            {
                // Dark blue
                r = 2.0f / 255.0f;
                g = 62.0f / 255.0f;
                b = 99.0f / 255.0f;
            }

            // 4 vertices per square (just a flat quad)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);

            vertices.push_back(x + squareSize);
            vertices.push_back(y);
            vertices.push_back(z);
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);

            vertices.push_back(x + squareSize);
            vertices.push_back(y);
            vertices.push_back(z + squareSize);
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z + squareSize);
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);

            // Indices for this square (2 triangles)
            indices.push_back(vertexCount + 0);
            indices.push_back(vertexCount + 1);
            indices.push_back(vertexCount + 2);

            indices.push_back(vertexCount + 0);
            indices.push_back(vertexCount + 2);
            indices.push_back(vertexCount + 3);

            vertexCount += 4;
        }
    }

    m_boardIndexCount = indices.size();

    glGenVertexArrays(1, &m_boardVAO);
    glGenBuffers(1, &m_boardVBO);
    glGenBuffers(1, &m_boardEBO);

    glBindVertexArray(m_boardVAO);

    // Position buffer (interleaved with color for simplicity we use separate buffers)
    glBindBuffer(GL_ARRAY_BUFFER, m_boardVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Color data
    GLuint colorVBO;
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_boardEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glBindBuffer(GL_ARRAY_BUFFER, m_boardVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

bool Renderer3D::createPieceGeometry()
{
    // Create a simple cube geometry for pieces
    std::vector<float> vertices = {
        // Front face
        -0.4f,
        -0.4f,
        0.4f,
        0.0f,
        0.0f,
        1.0f,
        0.4f,
        -0.4f,
        0.4f,
        0.0f,
        0.0f,
        1.0f,
        0.4f,
        0.4f,
        0.4f,
        0.0f,
        0.0f,
        1.0f,
        -0.4f,
        0.4f,
        0.4f,
        0.0f,
        0.0f,
        1.0f,
        // Back face
        -0.4f,
        -0.4f,
        -0.4f,
        0.0f,
        0.0f,
        -1.0f,
        0.4f,
        -0.4f,
        -0.4f,
        0.0f,
        0.0f,
        -1.0f,
        0.4f,
        0.4f,
        -0.4f,
        0.0f,
        0.0f,
        -1.0f,
        -0.4f,
        0.4f,
        -0.4f,
        0.0f,
        0.0f,
        -1.0f,
        // Right face
        0.4f,
        -0.4f,
        -0.4f,
        1.0f,
        0.0f,
        0.0f,
        0.4f,
        -0.4f,
        0.4f,
        1.0f,
        0.0f,
        0.0f,
        0.4f,
        0.4f,
        0.4f,
        1.0f,
        0.0f,
        0.0f,
        0.4f,
        0.4f,
        -0.4f,
        1.0f,
        0.0f,
        0.0f,
        // Left face
        -0.4f,
        -0.4f,
        -0.4f,
        -1.0f,
        0.0f,
        0.0f,
        -0.4f,
        -0.4f,
        0.4f,
        -1.0f,
        0.0f,
        0.0f,
        -0.4f,
        0.4f,
        0.4f,
        -1.0f,
        0.0f,
        0.0f,
        -0.4f,
        0.4f,
        -0.4f,
        -1.0f,
        0.0f,
        0.0f,
        // Bottom face
        -0.4f,
        -0.4f,
        -0.4f,
        0.0f,
        -1.0f,
        0.0f,
        0.4f,
        -0.4f,
        -0.4f,
        0.0f,
        -1.0f,
        0.0f,
        0.4f,
        -0.4f,
        0.4f,
        0.0f,
        -1.0f,
        0.0f,
        -0.4f,
        -0.4f,
        0.4f,
        0.0f,
        -1.0f,
        0.0f,
        // Top face
        -0.4f,
        0.4f,
        -0.4f,
        0.0f,
        1.0f,
        0.0f,
        0.4f,
        0.4f,
        -0.4f,
        0.0f,
        1.0f,
        0.0f,
        0.4f,
        0.4f,
        0.4f,
        0.0f,
        1.0f,
        0.0f,
        -0.4f,
        0.4f,
        0.4f,
        0.0f,
        1.0f,
        0.0f,
    };

    std::vector<unsigned int> indices = {
        // Front face
        0,
        1,
        2,
        0,
        2,
        3,
        // Back face
        4,
        6,
        5,
        4,
        7,
        6,
        // Right face
        8,
        9,
        10,
        8,
        10,
        11,
        // Left face
        12,
        14,
        13,
        12,
        15,
        14,
        // Bottom face
        16,
        18,
        17,
        16,
        19,
        18,
        // Top face
        20,
        21,
        22,
        20,
        22,
        23,
    };

    m_pieceIndexCount = indices.size();

    glGenVertexArrays(1, &m_pieceVAO);
    glGenBuffers(1, &m_pieceVBO);
    glGenBuffers(1, &m_pieceEBO);

    glBindVertexArray(m_pieceVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_pieceVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pieceEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute (float, float, float + normal float, float, float)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void Renderer3D::deletePlaneGeometry()
{
    // Cleanup if needed
}

void Renderer3D::render(const GameState& gameState)
{
    // Update animations
    updateAnimation();

    // Update piece camera if a cell is selected
    if (gameState.isCellSelected())
    {
        auto [selectedRow, selectedCol] = gameState.getSelectedCell();
        // Convert board coordinates (row, col) to 3D position
        // Position at board position - camera at top of piece
        glm::vec3 piecePosition(selectedCol + 0.5f, 1.5f, (7 - selectedRow) + 0.5f);
        m_pieceCamera.setPiecePosition(piecePosition);
    }

    // Bind framebuffer and render to texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);

    // Clear the framebuffer
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);  // Normal blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Get view-projection matrix from camera
    glm::mat4 viewProj = getViewProjectionMatrix();
    
    // Render skybox FIRST as background
    m_skybox->render(viewProj);

    // Render board with regular view projection
    drawBoard(gameState);

    // Render pieces
    drawPieces(gameState);

    // Render highlight overlays
    drawSelectedHighlight(gameState);
    drawPossibleMovesHighlight(gameState);

    glDisable(GL_DEPTH_TEST);

    // Unbind framebuffer (return to default)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer3D::drawBoard(const GameState& gameState)
{
    m_boardProgram->use();

    glm::mat4 viewProj = getViewProjectionMatrix();
    GLint     vpLoc    = glGetUniformLocation(m_boardProgram->getGLId(), "viewProjectionMatrix");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &viewProj[0][0]);

    glBindVertexArray(m_boardVAO);
    glDrawElements(GL_TRIANGLES, m_boardIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer3D::drawPieces(const GameState& gameState)
{
    m_pieceProgram->use();

    glm::mat4 viewProj = getViewProjectionMatrix();
    GLint     vpLoc    = glGetUniformLocation(m_pieceProgram->getGLId(), "viewProjectionMatrix");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &viewProj[0][0]);

    // Calculate camera position for specular calculation
    glm::vec3 cameraPosition(4.0f, 15.0f, 14.0f);  // Approximate camera position
    GLint camPosLoc = glGetUniformLocation(m_pieceProgram->getGLId(), "cameraPosition");
    glUniform3fv(camPosLoc, 1, &cameraPosition[0]);

    // Get current player and pass to shader
    Color currentPlayer = gameState.getCurrentPlayer();
    float playerValue = (currentPlayer == Color::White) ? 0.0f : 1.0f;
    GLint currentPlayerLoc = glGetUniformLocation(m_pieceProgram->getGLId(), "currentPlayer");
    glUniform1f(currentPlayerLoc, playerValue);

    const Board& board = gameState.getBoard();

    // Render each piece
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            const Piece* piece = board.getPieceAt(row, col);
            if (!piece)
                continue;

            // Position at board position - pieces sit on top of the squares
            glm::vec3 position(col + 0.5f, 0.32f, (7 - row) + 0.5f);
            
            // Adjust height for king and queen to be level with other pieces
            if (piece->getType() == PieceType::King || piece->getType() == PieceType::Queen)
            {
                position.y = 0.03f;  // Lower position for king/queen
            }

            // Handle piece animation if active and this is the destination piece
            if (m_pieceAnimationManager.hasActiveMovement())
            {
                const PieceMovement& anim = m_pieceAnimationManager.getActiveMovement();
                if (row == anim.toRow && col == anim.toCol)
                {
                    // This piece is being animated
                    glm::vec3 fromPos(anim.fromCol + 0.5f, 0.32f, (7 - anim.fromRow) + 0.5f);
                    glm::vec3 toPos = position;
                    
                    // Adjust from height for king/queen if needed
                    // (we need to know the piece type at the destination, which we already have)
                    if (piece->getType() == PieceType::King || piece->getType() == PieceType::Queen)
                    {
                        fromPos.y = 0.03f;
                        toPos.y = 0.03f;
                    }
                    
                    // Interpolate position based on animation progress
                    float progress = anim.getProgress();
                    position = fromPos + (toPos - fromPos) * progress;
                }
            }
            
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);
            
            // Rotate 90 degrees to the left for proper orientation
            modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            
            // Rotate pieces based on color: black pieces face opposite direction
            if (piece->getColor() == Color::Black)
            {
                modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            }
            
            // Scale down the piece models to fit properly on the board
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));

            GLint modelLoc = glGetUniformLocation(m_pieceProgram->getGLId(), "modelMatrix");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);

            // Set piece color based on player
            glm::vec3 pieceColor;
            if (m_daltonismMode)
            {
                // Daltonism colors (orange and blue)
                pieceColor = (piece->getColor() == Color::White)
                                 ? glm::vec3(1.0f, 0.5f, 0.0f)  // Orange
                                 : glm::vec3(0.0f, 0.5f, 1.0f); // Blue
            }
            else
            {
                // Normal colors
                pieceColor = (piece->getColor() == Color::White)
                                 ? glm::vec3(1.0f, 1.0f, 1.0f)
                                 : glm::vec3(0.2f, 0.2f, 0.2f);
            }

            GLint colorLoc = glGetUniformLocation(m_pieceProgram->getGLId(), "pieceColor");
            glUniform3fv(colorLoc, 1, &pieceColor[0]);

            // Use 3D models if available
            if (m_useModeledPieces && m_pieceModels)
            {
                // Get piece type name in lowercase
                std::string pieceName;
                switch (piece->getType())
                {
                    case PieceType::Pawn:
                        pieceName = "pawn";
                        break;
                    case PieceType::Knight:
                        pieceName = "knight";
                        break;
                    case PieceType::Bishop:
                        pieceName = "bishop";
                        break;
                    case PieceType::Rook:
                        pieceName = "rook";
                        break;
                    case PieceType::Queen:
                        pieceName = "queen";
                        break;
                    case PieceType::King:
                        pieceName = "king";
                        break;
                    default:
                        pieceName = "pawn";
                        break;
                }

                // Try to get the mesh for this piece type
                Mesh* mesh = m_pieceModels->getMesh(pieceName);
                if (mesh)
                {
                    // Render using 3D model
                    mesh->render();
                    continue;
                }
                // If mesh not found, fall through to cube rendering
            }

            // Fallback: render as cube
            glBindVertexArray(m_pieceVAO);
            glDrawElements(GL_TRIANGLES, m_pieceIndexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
}

void Renderer3D::drawSelectedHighlight(const GameState& gameState)
{
    // TODO: Implement highlighting for selected cell
    (void)gameState;  // Suppress unused parameter warning
}

void Renderer3D::drawPossibleMovesHighlight(const GameState& gameState)
{
    // TODO: Implement highlighting for possible moves
    (void)gameState;  // Suppress unused parameter warning
}

void Renderer3D::setViewportSize(int width, int height)
{
    if (width != m_framebufferWidth || height != m_framebufferHeight)
    {
        m_framebufferWidth  = width;
        m_framebufferHeight = height;

        // Recreate framebuffer with new size
        if (m_framebuffer != 0)
            glDeleteFramebuffers(1, &m_framebuffer);
        if (m_framebufferTexture != 0)
            glDeleteTextures(1, &m_framebufferTexture);
        if (m_depthRenderbuffer != 0)
            glDeleteRenderbuffers(1, &m_depthRenderbuffer);

        createFramebuffer(width, height);
    }
}

glm::mat4 Renderer3D::getViewProjectionMatrix() const
{
    glm::mat4 view, proj;
    float     aspect = static_cast<float>(m_framebufferWidth) / static_cast<float>(m_framebufferHeight);

    if (m_cameraMode == CameraMode::Trackball)
    {
        view = m_trackballCamera.getViewMatrix();
        // Center the trackball camera around the board center (4, 4, 0) instead of origin
        glm::vec3 boardCenter(4.0f, 0.0f, 4.0f);
        view = view * glm::translate(glm::mat4(1.0f), -boardCenter);
    }
    else if (m_cameraMode == CameraMode::Piece)
    {
        // Piece camera: positioned at the selected piece, looking around
        view = m_pieceCamera.getViewMatrix();
    }

    proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 200.0f);
    return proj * view;
}

void Renderer3D::setCameraMode(CameraMode mode)
{
    m_cameraMode = mode;
}

void Renderer3D::rotateTrackballLeft(float degrees)
{
    m_trackballCamera.rotateLeft(degrees);
}

void Renderer3D::rotateTrackballUp(float degrees)
{
    m_trackballCamera.rotateUp(degrees);
}

void Renderer3D::zoomTrackball(float delta)
{
    m_trackballCamera.moveFront(delta);
}

void Renderer3D::rotatePieceLeft(float degrees)
{
    m_pieceCamera.rotateLeft(degrees);
}

void Renderer3D::rotatePieceUp(float degrees)
{
    m_pieceCamera.rotateUp(degrees);
}

void Renderer3D::animatePieceMovement(int fromRow, int fromCol, int toRow, int toCol, float duration)
{
    m_pieceAnimationManager.startMovement(fromRow, fromCol, toRow, toCol, duration);
}

void Renderer3D::updateAnimation()
{
    m_pieceAnimationManager.update();
}

#include "Renderer3D.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "GLHeaders.hpp"
#include "core/Board.hpp"
#include "core/GameState.hpp"
#include "core/Piece.hpp"
#include "glimac/FilePath.hpp"

Renderer3D::Renderer3D()
    : m_framebuffer(0), m_framebufferTexture(0), m_depthRenderbuffer(0), m_framebufferWidth(800), m_framebufferHeight(800), m_boardVAO(0), m_boardVBO(0), m_boardEBO(0), m_boardIndexCount(0), m_pieceVAO(0), m_pieceVBO(0), m_pieceEBO(0), m_pieceIndexCount(0)
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
    m_camera.setViewport(width, height);

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

    // Initialize skybox
    m_skybox = std::make_unique<Skybox>();
    if (!m_skybox->initialize(m_executablePath))
    {
        std::cerr << "Failed to initialize skybox" << std::endl;
        return false;
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
        glimac::FilePath pieceVs(applicationPath.dirPath() + "../../assets/shaders/piece.vs.glsl");
        glimac::FilePath pieceFs(applicationPath.dirPath() + "../../assets/shaders/piece.fs.glsl");

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
    // Create 8x8 board with quads (light/dark squares)
    std::vector<float>        vertices;
    std::vector<float>        colors;
    std::vector<unsigned int> indices;

    float squareSize   = 1.0f;
    float boardStartX  = 0.0f;
    float boardStartZ  = 0.0f;
    float squareHeight = 0.1f; // Thin squares

    unsigned int vertexCount = 0;

    // Generate 64 squares (8x8)
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            float x = boardStartX + col * squareSize;
            float z = boardStartZ + (7 - row) * squareSize;
            float y = 0.0f;

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

            // 4 vertices per square
            // Top surface
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

            // Side surface (top edge)
            vertices.push_back(x);
            vertices.push_back(y + squareHeight);
            vertices.push_back(z);
            colors.push_back(r * 0.8f);
            colors.push_back(g * 0.8f);
            colors.push_back(b * 0.8f);

            vertices.push_back(x + squareSize);
            vertices.push_back(y + squareHeight);
            vertices.push_back(z);
            colors.push_back(r * 0.8f);
            colors.push_back(g * 0.8f);
            colors.push_back(b * 0.8f);

            vertices.push_back(x + squareSize);
            vertices.push_back(y + squareHeight);
            vertices.push_back(z + squareSize);
            colors.push_back(r * 0.8f);
            colors.push_back(g * 0.8f);
            colors.push_back(b * 0.8f);

            vertices.push_back(x);
            vertices.push_back(y + squareHeight);
            vertices.push_back(z + squareSize);
            colors.push_back(r * 0.8f);
            colors.push_back(g * 0.8f);
            colors.push_back(b * 0.8f);

            // Indices for this square's top (2 triangles)
            indices.push_back(vertexCount + 0);
            indices.push_back(vertexCount + 1);
            indices.push_back(vertexCount + 2);

            indices.push_back(vertexCount + 0);
            indices.push_back(vertexCount + 2);
            indices.push_back(vertexCount + 3);

            // Indices for side surface (front face)
            indices.push_back(vertexCount + 0);
            indices.push_back(vertexCount + 4);
            indices.push_back(vertexCount + 5);

            indices.push_back(vertexCount + 0);
            indices.push_back(vertexCount + 5);
            indices.push_back(vertexCount + 1);

            vertexCount += 8;
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
    // Bind framebuffer and render to texture
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glViewport(0, 0, m_framebufferWidth, m_framebufferHeight);

    // Clear the framebuffer
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Get view-projection matrix from camera
    glm::mat4 viewProj = m_camera.getViewProjectionMatrix();

    // Render skybox
    m_skybox->render(viewProj);

    // Render board
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

    glm::mat4 viewProj = m_camera.getViewProjectionMatrix();
    GLint     vpLoc    = glGetUniformLocation(m_boardProgram->getGLId(), "viewProjectionMatrix");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &viewProj[0][0]);

    glBindVertexArray(m_boardVAO);
    glDrawElements(GL_TRIANGLES, m_boardIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Renderer3D::drawPieces(const GameState& gameState)
{
    m_pieceProgram->use();

    glm::mat4 viewProj = m_camera.getViewProjectionMatrix();
    GLint     vpLoc    = glGetUniformLocation(m_pieceProgram->getGLId(), "viewProjectionMatrix");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, &viewProj[0][0]);

    const Board& board = gameState.getBoard();

    // Render each piece as a cube
    for (int row = 0; row < 8; ++row)
    {
        for (int col = 0; col < 8; ++col)
        {
            const Piece* piece = board.getPieceAt(row, col);
            if (!piece)
                continue;

            // Position cube at board position
            glm::vec3 position(col + 0.5f, 0.5f, (7 - row) + 0.5f);
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position);

            GLint modelLoc = glGetUniformLocation(m_pieceProgram->getGLId(), "modelMatrix");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &modelMatrix[0][0]);

            // Set piece color based on player
            glm::vec3 pieceColor;
            if (piece->getColor() == Color::White)
            {
                pieceColor = glm::vec3(1.0f, 1.0f, 1.0f); // White
            }
            else
            {
                pieceColor = glm::vec3(0.2f, 0.2f, 0.2f); // Black
            }

            GLint colorLoc = glGetUniformLocation(m_pieceProgram->getGLId(), "pieceColor");
            glUniform3fv(colorLoc, 1, &pieceColor[0]);

            glBindVertexArray(m_pieceVAO);
            glDrawElements(GL_TRIANGLES, m_pieceIndexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
}

void Renderer3D::drawSelectedHighlight(const GameState& gameState)
{
    // TODO: Implement highlighting for selected cell
    (void)gameState; // Suppress unused parameter warning
}

void Renderer3D::drawPossibleMovesHighlight(const GameState& gameState)
{
    // TODO: Implement highlighting for possible moves
    (void)gameState; // Suppress unused parameter warning
}

void Renderer3D::setViewportSize(int width, int height)
{
    if (width != m_framebufferWidth || height != m_framebufferHeight)
    {
        m_framebufferWidth  = width;
        m_framebufferHeight = height;
        m_camera.setViewport(width, height);

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

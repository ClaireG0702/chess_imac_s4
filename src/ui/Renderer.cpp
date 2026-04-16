#include "Renderer.hpp"
#include <imgui.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include "../core/Piece.hpp"
#include "../core/pieces/Bishop.hpp"
#include "../core/pieces/King.hpp"
#include "../core/pieces/Knight.hpp"
#include "../core/pieces/Pawn.hpp"
#include "../core/pieces/Queen.hpp"
#include "../core/pieces/Rook.hpp"
#include "core/GameState.hpp"

static const char* getPieceSymbol(const Piece* piece)
{
    if (!piece)
        return " ";
    Color color = piece->getColor();
    if (dynamic_cast<const Pawn*>(piece))
    {
        return color == Color::White ? "♙" : "♟";
    }
    else if (dynamic_cast<const Rook*>(piece))
    {
        return color == Color::White ? "♖" : "♜";
    }
    else if (dynamic_cast<const Knight*>(piece))
    {
        return color == Color::White ? "♘" : "♞";
    }
    else if (dynamic_cast<const Bishop*>(piece))
    {
        return color == Color::White ? "♗" : "♝";
    }
    else if (dynamic_cast<const Queen*>(piece))
    {
        return color == Color::White ? "♕" : "♛";
    }
    else if (dynamic_cast<const King*>(piece))
    {
        return color == Color::White ? "♔" : "♚";
    }
    return "?";
}

Renderer::Renderer() : m_cellSize(95.0f), m_boardOffset(0.f, 0.f), m_lightCellColor(IM_COL32(173, 232, 244, 255)), m_darkCellColor(IM_COL32(2, 62, 99, 138)), m_selectedColor(IM_COL32(255, 255, 0, 128)), m_possibleMoveColor(IM_COL32(0, 255, 0, 128)) {}

bool Renderer::initialize(const std::string& executablePath)
{
    // Initialize 3D renderer
    m_renderer3D = std::make_unique<Renderer3D>();
    if (!m_renderer3D->initialize(executablePath, 600, 600))
    {
        return false;
    }
    m_renderer3DInitialized = true;
    return true;
}

void Renderer::shutdown()
{
    // Cleanup if necessary
}

void Renderer::render(GameState& gameState)
{
    // Render 3D view in left panel
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(650, 700), ImGuiCond_FirstUseEver);
    ImGui::Begin("3D Chess View", nullptr, ImGuiWindowFlags_NoMove);

    render3DView(gameState);

    ImGui::End();

    // Render 2D controls in right panel
    ImGui::SetNextWindowPos(ImVec2(650, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(250, 700), ImGuiCond_FirstUseEver);
    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_NoMove);

    // Obtenir la taille disponible de la fenêtre
    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    float  boardSize     = std::min(availableSize.x, availableSize.y - 150.0f);
    float  cellSize      = boardSize / 8.0f;

    // Enlever l'espacement entre les boutons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    renderBoard(gameState, cellSize);

    ImGui::PopStyleVar(2);

    renderPromotionModal(gameState);
    renderResetConfirmationModal(gameState);

    renderGameStatus(gameState);
    renderEventHistory(gameState);

    ImGui::End();
}

std::vector<std::pair<int, int>> Renderer::getPossibleMoves(GameState& gameState) const
{
    std::vector<std::pair<int, int>> moves;
    if (gameState.isCellSelected())
    {
        auto [selRow, selCol] = gameState.getSelectedCell();
        Piece* selectedPiece  = gameState.getBoard().getPieceAt(selRow, selCol);
        if (selectedPiece)
        {
            moves = selectedPiece->getPossibleMoves(selRow, selCol, gameState.getBoard());
        }
    }
    return moves;
}

ImU32 Renderer::getCellColor(bool isLight, bool isSelected, bool isPossibleMove) const
{
    if (isSelected)
    {
        return m_selectedColor;
    }
    else if (isPossibleMove)
    {
        return m_possibleMoveColor;
    }
    return isLight ? m_lightCellColor : m_darkCellColor;
}

std::pair<ImVec4, ImVec4> Renderer::getHoverAndClickColors(ImVec4 baseVec) const
{
    ImVec4 hover = baseVec;
    hover.x      = (hover.x + 0.08f > 1.f) ? 1.f : hover.x + 0.08f;
    hover.y      = (hover.y + 0.08f > 1.f) ? 1.f : hover.y + 0.08f;
    hover.z      = (hover.z + 0.08f > 1.f) ? 1.f : hover.z + 0.08f;

    ImVec4 click = baseVec;
    click.x      = (click.x - 0.08f < 0.f) ? 0.f : click.x - 0.08f;
    click.y      = (click.y - 0.08f < 0.f) ? 0.f : click.y - 0.08f;
    click.z      = (click.z - 0.08f < 0.f) ? 0.f : click.z - 0.08f;

    return {hover, click};
}
void Renderer::renderCell(GameState& gameState, int x, int y, ImVec2 size, const std::vector<std::pair<int, int>>& possibleMoves)
{
    ImGui::PushID(y * 8 + x);

    bool light          = ((x + y) % 2 == 0);
    bool isSelected     = gameState.isCellSelected() && gameState.getSelectedCell() == std::make_pair(y, x);
    bool isPossibleMove = std::find(possibleMoves.begin(), possibleMoves.end(), std::make_pair(y, x)) != possibleMoves.end();

    const Piece* piece  = gameState.getBoard().getPieceAt(y, x);
    const char*  symbol = getPieceSymbol(piece);

    ImU32 baseColor = getCellColor(light, isSelected, isPossibleMove);
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(baseColor));

    ImVec4 baseVec      = ImGui::ColorConvertU32ToFloat4(baseColor);
    auto [hover, click] = getHoverAndClickColors(baseVec);

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, click);

    if (piece)
    {
        if (piece->getColor() == Color::White)
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
        else
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(30, 30, 30, 255));
    }

    if (ImGui::Button(symbol, size))
    {
        // Use unified cell click handler
        gameState.handleCellClick(y, x);
    }

    if (piece)
        ImGui::PopStyleColor();
    ImGui::PopStyleColor(3);
    ImGui::PopID();

    if (x < 7)
        ImGui::SameLine();
}

void Renderer::renderBoard(GameState& gameState, float cellSize)
{
    ImVec2                           size{cellSize, cellSize};
    std::vector<std::pair<int, int>> possibleMoves = getPossibleMoves(gameState);

    for (int y = 7; y >= 0; --y)
    {
        for (int x = 0; x < 8; ++x)
        {
            renderCell(gameState, x, y, size, possibleMoves);
        }
    }
}

void Renderer::renderGameStatus(GameState& gameState)
{
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const char* currentPlayer = (gameState.getCurrentPlayer() == Color::White) ? "Blancs" : "Noirs";
    ImGui::Text("Tour de : %s", currentPlayer);

    switch (gameState.getStatus())
    {
    case GameStatus::Playing:
        ImGui::Text("En cours...");
        break;
    case GameStatus::Check:
        ImGui::Text("Échec!");
        break;
    case GameStatus::Checkmate:
        ImGui::Text("Échec et mat! ");
        break;
    case GameStatus::Stalemate:
        ImGui::Text("Pat! Match nul.");
        break;
    case GameStatus::Victory:
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f)); // Green text
        ImGui::TextUnformatted("========================================");
        ImGui::Text("VICTOIRE! Le roi adverse a été capturé!");
        ImGui::Text("La partie est terminée.");
        ImGui::TextUnformatted("========================================");
        ImGui::PopStyleColor();
        break;
    }
    default:
        break;
    }

    if (ImGui::Button("Nouvelle partie"))
    {
        m_resetConfirmationRequested = true;
    }
}

std::pair<int, int> Renderer::getCellFromMousePosition(const ImVec2& mousePos) const
{
    ImVec2 windowPos   = ImGui::GetWindowPos();
    ImVec2 relativePos = ImVec2(mousePos.x - windowPos.x - m_boardOffset.x, mousePos.y - windowPos.y - m_boardOffset.y);

    int col = static_cast<int>(relativePos.x / m_cellSize);
    int row = static_cast<int>(relativePos.y / m_cellSize);

    return {row, col};
}

void Renderer::renderPromotionModal(GameState& gameState)
{
    if (!gameState.isPromotionPending())
    {
        return;
    }

    ImGui::OpenPopup("Pawn Promotion");

    if (ImGui::BeginPopupModal("Pawn Promotion", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Choisissez la pièce pour la promotion :");
        ImGui::Spacing();

        auto [row, col] = gameState.getPromotionPosition();

        if (ImGui::Button("Dame##promote", ImVec2(120, 0)))
        {
            gameState.promotePawn(row, col, PieceType::Queen);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Tour##promote", ImVec2(120, 0)))
        {
            gameState.promotePawn(row, col, PieceType::Rook);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Fou##promote", ImVec2(120, 0)))
        {
            gameState.promotePawn(row, col, PieceType::Bishop);
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Cavalier##promote", ImVec2(120, 0)))
        {
            gameState.promotePawn(row, col, PieceType::Knight);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Renderer::renderResetConfirmationModal(GameState& gameState)
{
    if (m_resetConfirmationRequested)
    {
        ImGui::OpenPopup("Confirmation de Réinitialisation");
        m_resetConfirmationRequested = false;
    }

    if (ImGui::BeginPopupModal("Confirmation de Réinitialisation", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Êtes-vous sûr de vouloir commencer une nouvelle partie?");
        ImGui::Text("La partie en cours sera perdue.");
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Oui, nouvelle partie##confirm", ImVec2(150, 0)))
        {
            gameState.initialize();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();

        if (ImGui::Button("Annuler##confirm", ImVec2(150, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void Renderer::render3DView(GameState& gameState)
{
    if (!m_renderer3DInitialized || !m_renderer3D)
    {
        ImGui::Text("3D Renderer not initialized");
        return;
    }

    // Render the 3D scene
    m_renderer3D->render(gameState);

    // Get the framebuffer texture and display it
    GLuint fbTexture = m_renderer3D->getFramebufferTexture();

    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    ImVec2 displaySize(availableSize.x, availableSize.y);

    // Update 3D viewport if size changed
    static ImVec2 lastSize(0, 0);
    if (displaySize.x != lastSize.x || displaySize.y != lastSize.y)
    {
        m_renderer3D->setViewportSize(static_cast<int>(displaySize.x), static_cast<int>(displaySize.y));
        lastSize = displaySize;
    }

    // Get cursor position and window position for ray-casting
    ImVec2 imagePos = ImGui::GetCursorScreenPos();
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 relativeMousePos(mousePos.x - imagePos.x, mousePos.y - imagePos.y);
    
    // Check if mouse is within the 3D view
    bool mouseInView = (relativeMousePos.x >= 0 && relativeMousePos.x < displaySize.x &&
                        relativeMousePos.y >= 0 && relativeMousePos.y < displaySize.y);
    
    // Display the framebuffer texture FIRST
    // Note: OpenGL texture coordinates are flipped vertically for ImGui
    ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<intptr_t>(fbTexture)), displaySize, ImVec2(0, 1), ImVec2(1, 0));
    
    // Check if mouse is hovering over the image
    bool isImageHovered = ImGui::IsItemHovered();
    
    // DEBUG: Always log mouse state
    static int logCounter = 0;
    if (logCounter++ % 30 == 0)
    {
        std::cout << "MOUSE_STATE: inView=" << mouseInView << " hovered=" << isImageHovered 
                  << " pos=(" << relativeMousePos.x << "," << relativeMousePos.y << ") clicked=" 
                  << ImGui::IsMouseClicked(ImGuiMouseButton_Left) << std::endl;
    }
    
    // Handle input only when image is hovered
    if (isImageHovered && mouseInView)
    {
        // Update hoverable cell based on mouse position (for ray-casting)
        m_renderer3D->updateHoverableCell(relativeMousePos.x, relativeMousePos.y);
        
        // Handle left-click for selection and movement
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            auto [row, col] = m_renderer3D->getCellFromMousePosition(relativeMousePos.x, relativeMousePos.y);
            
            // DEBUG OUTPUT
            std::cout << "✓ CLICK DETECTED at relative pos (" << relativeMousePos.x << ", " << relativeMousePos.y << ")" << std::endl;
            std::cout << "✓ Ray-cast returned cell: (" << row << ", " << col << ")" << std::endl;
            
            if (!gameState.getBoard().isValidPosition(row, col) || gameState.getStatus() == GameStatus::Victory)
            {
                std::cout << "DEBUG: Invalid position or game status prevents move" << std::endl;
                return; // Early exit for invalid position or victory
            }

            // Store selected state before click to know if we need animation
            auto [selectedRow, selectedCol] = gameState.getSelectedCell();
            bool wasSelected = gameState.isCellSelected();

            // Use unified cell click handler
            ClickResult result = gameState.handleCellClick(row, col);

            // If move was executed, animate it
            if (result.action == ClickAction::MoveExecuted && wasSelected)
            {
                m_renderer3D->animatePieceMovement(selectedRow, selectedCol, row, col, 0.8f);
            }

            // Debug output
            switch (result.action)
            {
                case ClickAction::MoveExecuted:
                    std::cout << "DEBUG: Move executed" << std::endl;
                    break;
                case ClickAction::Selected:
                    std::cout << "DEBUG: Selected cell (" << row << ", " << col << ")" << std::endl;
                    break;
                case ClickAction::Deselected:
                    std::cout << "DEBUG: Deselected cell" << std::endl;
                    break;
                case ClickAction::None:
                    if (result.hasError)
                        std::cout << "DEBUG: Click error: " << result.errorMessage << std::endl;
                    break;
            }
        }
        
        // Handle right-click to deselect
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            gameState.deselectCell();
            std::cout << "DEBUG: Right-click, deselected" << std::endl;
        }
    }
}

void Renderer::renderEventHistory(GameState& gameState)
{
    if (gameState.getGameMode() != GameMode::Chaotic)
    {
        return; // Only show history in chaotic mode
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Historique des événements:");

    // Use a child window for scrollable event list
    ImVec2 childSize = ImGui::GetContentRegionAvail();
    childSize.y      = std::max(80.0f, childSize.y - 20.0f);

    ImGui::BeginChild("EventHistory", childSize, true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    const auto& eventHistory = gameState.getEventHistory();

    if (eventHistory.empty())
    {
        ImGui::TextDisabled("Aucun événement pour le moment...");
    }
    else
    {
        // Display events in reverse order (newest first)
        for (int i = static_cast<int>(eventHistory.size()) - 1; i >= 0; --i)
        {
            const auto& event = eventHistory[i];

            ImGui::Text("[Tour %d] %s", event.turnNumber, event.eventName.c_str());
        }
    }

    // Auto-scroll to bottom if new events
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
}

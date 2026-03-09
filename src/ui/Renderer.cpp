#include "Renderer.hpp"
#include <imgui.h>
#include "core/GameState.hpp"
#include "../core/Piece.hpp"
#include "../core/pieces/Pawn.hpp"
#include "../core/pieces/Rook.hpp"
#include "../core/pieces/Knight.hpp"
#include "../core/pieces/Bishop.hpp"
#include "../core/pieces/Queen.hpp"
#include "../core/pieces/King.hpp"
#include <algorithm>
#include <vector>

static const char* getPieceSymbol(const Piece* piece) {
    if (!piece) return " ";
    
    Color color = piece->getColor();

    // piece->toString();
    
    // Try to determine piece type using dynamic_cast
    if (dynamic_cast<const Pawn*>(piece)) {
        return color == Color::White ? "PB" : "PN";
    } else if (dynamic_cast<const Rook*>(piece)) {
        return color == Color::White ? "TB" : "TN";
    } else if (dynamic_cast<const Knight*>(piece)) {
        return color == Color::White ? "CB" : "CN";
    } else if (dynamic_cast<const Bishop*>(piece)) {
        return color == Color::White ? "FB" : "FN";
    } else if (dynamic_cast<const Queen*>(piece)) {
        return color == Color::White ? "DB" : "DN";
    } else if (dynamic_cast<const King*>(piece)) {
        return color == Color::White ? "RB" : "RN";
    }
    
    return "?";
}

Renderer::Renderer() : m_cellSize(95.0f), m_boardOffset(0.f, 0.f),
    m_lightCellColor(IM_COL32(173, 232, 244, 255)), 
    m_darkCellColor(IM_COL32(2, 62, 99, 138)),
    m_selectedColor(IM_COL32(255, 255, 0, 128)), 
    m_possibleMoveColor(IM_COL32(0, 255, 0, 128)) {}

bool Renderer::initialize() {
    return true;
}

void Renderer::shutdown() {
    // Cleanup if necessary
}

void Renderer::render(GameState& gameState) {
    ImGui::SetNextWindowSize(ImVec2(800, 800), ImGuiCond_FirstUseEver);
    ImGui::Begin("Chess Game", nullptr, ImGuiWindowFlags_NoResize);

    // Obtenir la taille disponible de la fenêtre
    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    float boardSize = std::min(availableSize.x, availableSize.y - 100.0f);
    float cellSize = boardSize / 8.0f;

    // Enlever l'espacement entre les boutons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    renderBoard(gameState, cellSize);

    ImGui::PopStyleVar(2);

    renderGameStatus(gameState);

    ImGui::End();
}

std::vector<std::pair<int, int>> Renderer::getPossibleMoves(GameState& gameState) const {
    std::vector<std::pair<int, int>> moves;
    if (gameState.isCellSelected()) {
        auto [selRow, selCol] = gameState.getSelectedCell();
        Piece* selectedPiece = gameState.getBoard().getPieceAt(selRow, selCol);
        if (selectedPiece) {
            moves = selectedPiece->getPossibleMoves(selRow, selCol, gameState.getBoard());
        }
    }
    return moves;
}

ImU32 Renderer::getCellColor(bool isLight, bool isSelected, bool isPossibleMove) const {
    if (isSelected) {
        return m_selectedColor;
    } else if (isPossibleMove) {
        return m_possibleMoveColor;
    }
    return isLight ? m_lightCellColor : m_darkCellColor;
}

std::pair<ImVec4, ImVec4> Renderer::getHoverAndClickColors(ImVec4 baseVec) const {
    ImVec4 hover = baseVec;
    hover.x = (hover.x + 0.08f > 1.f) ? 1.f : hover.x + 0.08f;
    hover.y = (hover.y + 0.08f > 1.f) ? 1.f : hover.y + 0.08f;
    hover.z = (hover.z + 0.08f > 1.f) ? 1.f : hover.z + 0.08f;

    ImVec4 click = baseVec;
    click.x = (click.x - 0.08f < 0.f) ? 0.f : click.x - 0.08f;
    click.y = (click.y - 0.08f < 0.f) ? 0.f : click.y - 0.08f;
    click.z = (click.z - 0.08f < 0.f) ? 0.f : click.z - 0.08f;

    return {hover, click};
}

void Renderer::renderCell(GameState& gameState, int x, int y, ImVec2 size, const std::vector<std::pair<int, int>>& possibleMoves) {
    ImGui::PushID(y * 8 + x);

    bool light = ((x + y) % 2 == 0);
    bool isSelected = gameState.isCellSelected() && gameState.getSelectedCell() == std::make_pair(y, x);
    bool isPossibleMove = std::find(possibleMoves.begin(), possibleMoves.end(), std::make_pair(y, x)) != possibleMoves.end();

    ImU32 baseColor = getCellColor(light, isSelected, isPossibleMove);
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(baseColor));

    ImVec4 baseVec = ImGui::ColorConvertU32ToFloat4(baseColor);
    auto [hover, click] = getHoverAndClickColors(baseVec);

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, click);

    const Piece* piece = gameState.getBoard().getPieceAt(y, x);
    const char* symbol = getPieceSymbol(piece);

    if (ImGui::Button(symbol, size)) {
        if (isPossibleMove && gameState.isCellSelected()) {
            auto [fromRow, fromCol] = gameState.getSelectedCell();
            gameState.makeMove(fromRow, fromCol, y, x);
            gameState.deselectCell();
        } else {
            gameState.selectCell(y, x);
        }
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();

    if (x < 7)
        ImGui::SameLine();
}

void Renderer::renderBoard(GameState& gameState, float cellSize) {
    ImVec2 size{cellSize, cellSize};
    std::vector<std::pair<int, int>> possibleMoves = getPossibleMoves(gameState);

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            renderCell(gameState, x, y, size, possibleMoves);
        }
    }
}

void Renderer::renderGameStatus(const GameState& gameState) {
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    const char* currentPlayer = (gameState.getCurrentPlayer() == Color::White) ? "Blancs" : "Noirs";
    ImGui::Text("Tour de : %s", currentPlayer);

    switch(gameState.getStatus()) {
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
        default:
            break;
    }

    if(ImGui::Button("Nouvelle partie")) {
        // Reset the game state
        // (This should ideally be handled by a method in GameState, but for simplicity we reinitialize it here)
        GameState newGameState;
        newGameState.initialize();
    }
}

std::pair<int, int> Renderer::getCellFromMousePosition(const ImVec2& mousePos) const {
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 relativePos = ImVec2(mousePos.x - windowPos.x - m_boardOffset.x, mousePos.y - windowPos.y - m_boardOffset.y);

    int col = static_cast<int>(relativePos.x / m_cellSize);
    int row = static_cast<int>(relativePos.y / m_cellSize);

    return {row, col};
}

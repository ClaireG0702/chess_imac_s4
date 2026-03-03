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
        return color == Color::White ? "PN" : "PB";
    } else if (dynamic_cast<const Rook*>(piece)) {
        return color == Color::White ? "TN" : "TB";
    } else if (dynamic_cast<const Knight*>(piece)) {
        return color == Color::White ? "CN" : "CB";
    } else if (dynamic_cast<const Bishop*>(piece)) {
        return color == Color::White ? "FN" : "FB";
    } else if (dynamic_cast<const Queen*>(piece)) {
        return color == Color::White ? "DN" : "DB";
    } else if (dynamic_cast<const King*>(piece)) {
        return color == Color::White ? "RN" : "RB";
    }
    
    return "?";
}

static std::vector<std::pair<int, int>> getPossibleMovesForSelection(const GameState& gameState) {
    std::vector<std::pair<int, int>> possibleMoves;
    if (gameState.isCellSelected()) {
        auto [selRow, selCol] = gameState.getSelectedCell();
        Piece* selectedPiece = gameState.getBoard().getPieceAt(selRow, selCol);
        if (selectedPiece) {
            possibleMoves = selectedPiece->getPossibleMoves(selRow, selCol, gameState.getBoard());
        }
    }
    return possibleMoves;
}

static ImU32 getCellColor(int x, int y, const GameState& gameState, 
                          const std::vector<std::pair<int, int>>& possibleMoves, 
                          ImU32 lightColor, ImU32 darkColor, ImU32 selectedColor, ImU32 moveColor) {
    bool light = ((x + y) % 2 == 0);
    bool isSelected = gameState.isCellSelected() && gameState.getSelectedCell() == std::make_pair(y, x);
    bool isPossibleMove = std::find(possibleMoves.begin(), possibleMoves.end(), std::make_pair(y, x)) != possibleMoves.end();

    if (isSelected) return selectedColor;
    if (isPossibleMove) return moveColor;
    return light ? lightColor : darkColor;
}

static void getColorVariations(ImU32 baseColor, ImVec4& hover, ImVec4& click) {
    ImVec4 baseVec = ImGui::ColorConvertU32ToFloat4(baseColor);
    hover = baseVec;
    hover.x = (hover.x + 0.08f > 1.f) ? 1.f : hover.x + 0.08f;
    hover.y = (hover.y + 0.08f > 1.f) ? 1.f : hover.y + 0.08f;
    hover.z = (hover.z + 0.08f > 1.f) ? 1.f : hover.z + 0.08f;

    click = baseVec;
    click.x = (click.x - 0.08f < 0.f) ? 0.f : click.x - 0.08f;
    click.y = (click.y - 0.08f < 0.f) ? 0.f : click.y - 0.08f;
    click.z = (click.z - 0.08f < 0.f) ? 0.f : click.z - 0.08f;
}

static void handleCellClick(int x, int y, GameState& gameState, 
                           const std::vector<std::pair<int, int>>& possibleMoves) {
    bool isPossibleMove = std::find(possibleMoves.begin(), possibleMoves.end(), std::make_pair(y, x)) != possibleMoves.end();
    
    if (isPossibleMove && gameState.isCellSelected()) {
        auto [fromRow, fromCol] = gameState.getSelectedCell();
        gameState.makeMove(fromRow, fromCol, y, x);
        gameState.deselectCell();
    } else {
        gameState.selectCell(y, x);
    }
}

static void renderCell(int x, int y, const GameState& gameState, ImVec2 size,
                      const std::vector<std::pair<int, int>>& possibleMoves,
                      ImU32 lightColor, ImU32 darkColor, ImU32 selectedColor, ImU32 moveColor) {
    ImGui::PushID(y * 8 + x);

    ImU32 baseColor = getCellColor(x, y, gameState, possibleMoves, lightColor, darkColor, selectedColor, moveColor);
    ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(baseColor));

    ImVec4 hover, click;
    getColorVariations(baseColor, hover, click);

    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, hover);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, click);

    const Piece* piece = gameState.getBoard().getPieceAt(y, x);
    const char* symbol = getPieceSymbol(piece);

    if (ImGui::Button(symbol, size)) {
        handleCellClick(x, y, const_cast<GameState&>(gameState), possibleMoves);
    }

    ImGui::PopStyleColor(3);
    ImGui::PopID();

    if (x < 7)
        ImGui::SameLine();
}

static void renderBoardCells(GameState& gameState, ImVec2 size,
                            const std::vector<std::pair<int, int>>& possibleMoves,
                            ImU32 lightColor, ImU32 darkColor, ImU32 selectedColor, ImU32 moveColor) {
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            renderCell(x, y, gameState, size, possibleMoves, lightColor, darkColor, selectedColor, moveColor);
        }
    }
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

    // Calculer la taille du plateau et des cases
    ImVec2 availableSize = ImGui::GetContentRegionAvail();
    float boardSize = std::min(availableSize.x, availableSize.y - 100.0f);
    float cellSize = boardSize / 8.0f;
    ImVec2 cellSizeVec{cellSize, cellSize};

    // Récupérer les mouvements possibles
    std::vector<std::pair<int, int>> possibleMoves = getPossibleMovesForSelection(gameState);

    // Enlever l'espacement entre les boutons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    // Afficher le plateau
    renderBoardCells(gameState, cellSizeVec, possibleMoves, m_lightCellColor, m_darkCellColor, m_selectedColor, m_possibleMoveColor);

    ImGui::PopStyleVar(2);

    // Afficher le statut du jeu
    renderGameStatus(gameState);

    ImGui::End();
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

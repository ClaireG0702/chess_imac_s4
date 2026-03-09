#include "InputHandler.hpp"
#include <imgui.h>

InputHandler::InputHandler(GameState& gameState, Renderer& renderer) : m_gameState(gameState), m_renderer(renderer) {}

void InputHandler::handleInput() {
    // Handle right-click to deselect
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
        m_gameState.deselectCell();
        return;
    }
    
    handleMouseClick();
}

void InputHandler::handleMouseClick() {
    if(!ImGui::IsMouseClicked(0)) return;
    if(!ImGui::IsWindowHovered()) return;

    ImVec2 mousePos = ImGui::GetMousePos();
    auto [row, col] = m_renderer.getCellFromMousePosition(mousePos);

    if(!m_gameState.getBoard().isValidPosition(row, col)) return;

    if(!m_gameState.isCellSelected()) {
        m_gameState.selectCell(row, col);
    } else {
        auto [selectedRow, selectedCol] = m_gameState.getSelectedCell();
        if(row == selectedRow && col == selectedCol) {
            m_gameState.deselectCell();
        } else {
            if(m_gameState.makeMove(selectedRow, selectedCol, row, col)) {
                // Move successful, deselect the cell
                m_gameState.deselectCell();
            } else {
                // Invalid move, keep the cell selected
                m_gameState.deselectCell();
                m_gameState.selectCell(row, col); // Select the new cell if it's valid
            }
        }
    }

}
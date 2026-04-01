#include "InputHandler.hpp"
#include <imgui.h>
#include "Renderer3D.hpp"

InputHandler::InputHandler(GameState& gameState, Renderer& renderer) : m_gameState(gameState), m_renderer(renderer) {}

void InputHandler::handleInput() {
    handleCameraInput();
    
    // Handle right-click to deselect (only if not dragging camera)
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered()) {
        m_gameState.deselectCell();
        return;
    }
    
    handleMouseClick();
}

void InputHandler::handleCameraInput() {
    Renderer3D* renderer3D = m_renderer.getRenderer3D();
    if (!renderer3D) return;

    // Changement de caméra avec touches numériques
    if (ImGui::IsKeyPressed(ImGuiKey_1)) {
        renderer3D->setCameraMode(CameraMode::Trackball);
    } else if (ImGui::IsKeyPressed(ImGuiKey_2)) {
        renderer3D->setCameraMode(CameraMode::Freefly);
    }

    // Contrôles trackball avec la souris (clic droit glissé)
    if (renderer3D->getCameraMode() == CameraMode::Trackball) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            if (drag.x != 0.0f) {
                renderer3D->rotateTrackballLeft(-drag.x * 0.5f);
            }
            if (drag.y != 0.0f) {
                renderer3D->rotateTrackballUp(drag.y * 0.5f);
            }
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
        }
    }

    // Contrôles trackball (flèches)
    if (ImGui::IsKeyDown(ImGuiKey_LeftArrow)) {
        renderer3D->rotateTrackballLeft(2.0f);
    }
    if (ImGui::IsKeyDown(ImGuiKey_RightArrow)) {
        renderer3D->rotateTrackballLeft(-2.0f);
    }
    if (ImGui::IsKeyDown(ImGuiKey_UpArrow)) {
        renderer3D->rotateTrackballUp(2.0f);
    }
    if (ImGui::IsKeyDown(ImGuiKey_DownArrow)) {
        renderer3D->rotateTrackballUp(-2.0f);
    }

    // Zoom avec molette
    float wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0.0f) {
        renderer3D->zoomTrackball(-wheel * 0.5f);
    }

    // Contrôles freefly (ZQSD + Click droit souris)
    if (ImGui::IsKeyDown(ImGuiKey_Z)) {
        renderer3D->moveFreeflyFront(0.1f);
    }
    if (ImGui::IsKeyDown(ImGuiKey_S)) {
        renderer3D->moveFreeflyFront(-0.1f);
    }
    if (ImGui::IsKeyDown(ImGuiKey_Q)) {
        renderer3D->moveFreeflyLeft(0.1f);
    }
    if (ImGui::IsKeyDown(ImGuiKey_D)) {
        renderer3D->moveFreeflyLeft(-0.1f);
    }
}

void InputHandler::handleMouseClick() {
    if(!ImGui::IsMouseClicked(0)) return;
    if(!ImGui::IsWindowHovered()) return;
    
    // Don't allow any moves if game is over (Victory)
    if (m_gameState.getStatus() == GameStatus::Victory) {
        return;
    }

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
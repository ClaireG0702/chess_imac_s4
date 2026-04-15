#include "InputHandler.hpp"
#include <imgui.h>
#include "Renderer3D.hpp"

InputHandler::InputHandler(GameState& gameState, Renderer& renderer) : m_gameState(gameState), m_renderer(renderer) {}

void InputHandler::handleInput()
{
    handleCameraInput();

    // Handle right-click to deselect (only if not dragging camera)
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
    {
        m_gameState.deselectCell();
        return;
    }

    handleMouseClick();
}

void InputHandler::handleCameraInput()
{
    Renderer3D* renderer3D = m_renderer.getRenderer3D();
    if (!renderer3D)
        return;

    // Changement de caméra avec touches numériques
    if (ImGui::IsKeyPressed(ImGuiKey_1))
    {
        renderer3D->setCameraMode(CameraMode::Trackball);
    }
    else if (ImGui::IsKeyPressed(ImGuiKey_2))
    {
        // Switch to piece camera (only if a cell is selected)
        if (m_gameState.isCellSelected())
        {
            renderer3D->setCameraMode(CameraMode::Piece);
        }
    }

    // Contrôles trackball avec la souris (clic droit glissé)
    if (renderer3D->getCameraMode() == CameraMode::Trackball)
    {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            if (drag.x != 0.0f)
            {
                renderer3D->rotateTrackballLeft(-drag.x * 0.5f);
            }
            if (drag.y != 0.0f)
            {
                renderer3D->rotateTrackballUp(drag.y * 0.5f);
            }
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
        }

        // Contrôles trackball (flèches)
        if (ImGui::IsKeyDown(ImGuiKey_LeftArrow))
        {
            renderer3D->rotateTrackballLeft(2.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_RightArrow))
        {
            renderer3D->rotateTrackballLeft(-2.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_UpArrow))
        {
            renderer3D->rotateTrackballUp(2.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_DownArrow))
        {
            renderer3D->rotateTrackballUp(-2.0f);
        }

        // Zoom avec molette
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f)
        {
            renderer3D->zoomTrackball(-wheel * 0.5f);
        }
    }
    else if (renderer3D->getCameraMode() == CameraMode::Piece)
    {
        // Piece camera controls - can rotate around piece with mouse and arrow keys but not move
        
        // Contrôles piece camera avec la souris (clic droit glissé)
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
        {
            ImVec2 drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            if (drag.x != 0.0f)
            {
                renderer3D->rotatePieceLeft(-drag.x * 0.5f);
            }
            if (drag.y != 0.0f)
            {
                renderer3D->rotatePieceUp(drag.y * 0.5f);
            }
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
        }

        // Contrôles piece camera (flèches) - 360 degrees around and up/down without limits
        if (ImGui::IsKeyDown(ImGuiKey_LeftArrow))
        {
            renderer3D->rotatePieceLeft(2.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_RightArrow))
        {
            renderer3D->rotatePieceLeft(-2.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_UpArrow))
        {
            renderer3D->rotatePieceUp(2.0f);
        }
        if (ImGui::IsKeyDown(ImGuiKey_DownArrow))
        {
            renderer3D->rotatePieceUp(-2.0f);
        }
    }
}

void InputHandler::handleMouseClick()
{
    if (!ImGui::IsMouseClicked(0))
        return;
    if (!ImGui::IsWindowHovered())
        return;

    // Don't allow any moves if game is over (Victory)
    if (m_gameState.getStatus() == GameStatus::Victory)
    {
        return;
    }

    ImVec2 mousePos = ImGui::GetMousePos();
    auto [row, col] = m_renderer.getCellFromMousePosition(mousePos);

    if (!m_gameState.getBoard().isValidPosition(row, col))
        return;

    if (!m_gameState.isCellSelected())
    {
        m_gameState.selectCell(row, col);
    }
    else
    {
        auto [selectedRow, selectedCol] = m_gameState.getSelectedCell();
        if (row == selectedRow && col == selectedCol)
        {
            m_gameState.deselectCell();
        }
        else
        {
            if (m_gameState.makeMove(selectedRow, selectedCol, row, col))
            {
                // Move successful - animate the piece
                Renderer3D* renderer3D = m_renderer.getRenderer3D();
                if (renderer3D)
                {
                    renderer3D->animatePieceMovement(selectedRow, selectedCol, row, col, 1.8f);
                }
                m_gameState.deselectCell();
            }
            else
            {
                // Invalid move, keep the cell selected
                m_gameState.deselectCell();
                m_gameState.selectCell(row, col); // Select the new cell if it's valid
            }
        }
    }
}
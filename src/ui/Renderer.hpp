#pragma once
#include "../core/GameState.hpp"
#include "Renderer3D.hpp"
#include <imgui.h>
#include <memory>
#include <vector>

class Renderer {
    public:
        Renderer();
        ~Renderer() = default;
        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(Renderer&&) = delete;

        void render(GameState& gameState);
        bool initialize(const std::string& executablePath = "");
        void shutdown();

        std::pair<int, int> getCellFromMousePosition(const ImVec2& mousePos) const;
        
        // Access to 3D renderer for camera control
        Renderer3D* getRenderer3D() { return m_renderer3D.get(); }

    private:
        float m_cellSize;
        ImVec2 m_boardOffset;

        ImU32 m_lightCellColor;
        ImU32 m_darkCellColor;
        ImU32 m_selectedColor;
        ImU32 m_possibleMoveColor;
        
        bool m_resetConfirmationRequested = false;
        
        // 3D renderer
        std::unique_ptr<Renderer3D> m_renderer3D;
        bool m_renderer3DInitialized = false;

        void renderGameStatus(GameState& gameState);
        void renderBoard(GameState& gameState, float cellSize);
        void renderCell(GameState& gameState, int x, int y, ImVec2 size, const std::vector<std::pair<int, int>>& possibleMoves);
        void renderPromotionModal(GameState& gameState);
        void renderResetConfirmationModal(GameState& gameState);
        void render3DView(GameState& gameState);
        
        std::vector<std::pair<int, int>> getPossibleMoves(GameState& gameState) const;
        ImU32 getCellColor(bool isLight, bool isSelected, bool isPossibleMove) const;
        std::pair<ImVec4, ImVec4> getHoverAndClickColors(ImVec4 baseVec) const;
};
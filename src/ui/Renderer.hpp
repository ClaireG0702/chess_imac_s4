#pragma once
#include "../core/GameState.hpp"
#include <imgui.h>
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
        bool initialize();
        void shutdown();

        std::pair<int, int> getCellFromMousePosition(const ImVec2& mousePos) const;

    private:
        float m_cellSize;
        ImVec2 m_boardOffset;

        ImU32 m_lightCellColor;
        ImU32 m_darkCellColor;
        ImU32 m_selectedColor;
        ImU32 m_possibleMoveColor;

        void renderGameStatus(const GameState& gameState);
        void renderBoard(GameState& gameState, float cellSize);
        void renderCell(GameState& gameState, int x, int y, ImVec2 size, const std::vector<std::pair<int, int>>& possibleMoves);
        
        std::vector<std::pair<int, int>> getPossibleMoves(GameState& gameState) const;
        ImU32 getCellColor(bool isLight, bool isSelected, bool isPossibleMove) const;
        std::pair<ImVec4, ImVec4> getHoverAndClickColors(ImVec4 baseVec) const;
};
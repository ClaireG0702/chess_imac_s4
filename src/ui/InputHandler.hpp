#pragma once
#include "../core/GameState.hpp"
#include "Renderer.hpp"

class InputHandler {
    public:
        InputHandler(GameState& gameState, Renderer& renderer);
        
        void handleInput();

    private:
        GameState& m_gameState;
        Renderer& m_renderer;
        
        void handleMouseClick();

};
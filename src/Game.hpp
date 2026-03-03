#pragma once
#include "core/Board.hpp"
#include "ui/Renderer.hpp"
#include "ui/InputHandler.hpp"
#include <memory>

class Game {
    public:
        Game();
        ~Game();

        bool initialize();
        void run();
        void shutdown();

    private:
        std::unique_ptr<GameState> m_gameState;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<InputHandler> m_inputHandler;

        bool m_isRunning;

        void update();
        void render();
};

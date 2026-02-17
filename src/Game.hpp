#pragma once

#include "core/Board.hpp"

class Game {
public:
    Game();
    ~Game();

    void init();
    void update();
    void render();

    bool isRunning() const { return m_isRunning; }
    void quit() { m_isRunning = false; }

private:
    bool  m_isRunning;
    Board m_board;
};

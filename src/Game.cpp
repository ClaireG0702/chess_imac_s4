#include "Game.hpp"
#include "ui/Render.hpp"

Game::Game() : m_isRunning(true) {
}

Game::~Game() {
}

void Game::init() {
    m_board.initialize();
}

void Game::update() {
    // Update game logic here
}

void Game::render() {
    DrawBoard(m_board);
}

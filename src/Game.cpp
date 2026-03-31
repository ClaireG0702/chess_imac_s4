#include "Game.hpp"
#include <iostream>

Game::Game() : m_isRunning(false) {}

Game::~Game() {
    shutdown();
}

bool Game::initialize(const std::string& executablePath) {
    m_gameState = std::make_unique<GameState>();
    m_renderer = std::make_unique<Renderer>();
    m_inputHandler = std::make_unique<InputHandler>(*m_gameState, *m_renderer);

    if (!m_renderer->initialize(executablePath)) {
        std::cerr << "Failed to initialize renderer\n";
        return false;
    }

    m_isRunning = true;
    return true;
}

void Game::run() {
    update();
    render();
}

void Game::update() {
    m_inputHandler->handleInput();
}

void Game::render() {
    m_renderer->render(*m_gameState);
}

void Game::shutdown() {
    if (m_renderer) {
        m_renderer->shutdown();
    }
    m_renderer = nullptr;
}
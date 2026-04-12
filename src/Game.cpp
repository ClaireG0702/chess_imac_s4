#include "Game.hpp"
#include <imgui.h>
#include <iostream>


Game::Game() : m_isRunning(false), m_gameStarted(false) {}

Game::~Game()
{
    shutdown();
}

bool Game::initialize(const std::string& executablePath)
{
    m_gameState    = std::make_unique<GameState>();
    m_renderer     = std::make_unique<Renderer>();
    m_inputHandler = std::make_unique<InputHandler>(*m_gameState, *m_renderer);

    if (!m_renderer->initialize(executablePath))
    {
        std::cerr << "Failed to initialize renderer\n";
        return false;
    }

    m_isRunning = true;
    return true;
}

void Game::run()
{
    if (!m_gameStarted)
    {
        showStartMenu();
    }
    else
    {
        update();
        render();
    }
}

void Game::update()
{
    m_inputHandler->handleInput();
}

void Game::render()
{
    m_renderer->render(*m_gameState);
}

void Game::showStartMenu()
{
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f - 200, ImGui::GetIO().DisplaySize.y * 0.5f - 150));
    ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::Begin("Mode de Jeu", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

    ImGui::TextUnformatted("Bienvenue au Jeu d'Echecs!");
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("Choisissez le mode de jeu:");
    ImGui::Spacing();

    if (ImGui::Button("Mode Classique", ImVec2(350, 50)))
    {
        m_gameState->setGameMode(GameMode::Classic);
        m_gameState->initialize();
        m_gameStarted = true;
    }

    ImGui::Spacing();

    if (ImGui::Button("Mode Chaotique", ImVec2(350, 50)))
    {
        m_gameState->setGameMode(GameMode::Chaotic);
        m_gameState->initialize();
        m_gameStarted = true;
    }

    ImGui::End();
}

void Game::shutdown()
{
    if (m_renderer)
    {
        m_renderer->shutdown();
    }
    m_renderer = nullptr;
}
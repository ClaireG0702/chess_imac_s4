#pragma once

#include <functional>
#include "../math/Binomial.hpp"
#include "../math/ChiSquared.hpp"
#include "../math/Poisson.hpp"
#include "../math/UniformDiscrete.hpp"

enum class GameEventType {
    Revolution,        // Binomial - Révolution politique
    EpidemicTransform, // Poisson - SwapReinePion
    CavalierSauvage,   // UniformDiscrete - Sélection aléatoire
    Daltonism,         // ChiSquared - Confusion daltonienne (pièces grises)
};

class GameEvent {
public:
    GameEventType         type;
    std::function<void()> action;

    GameEvent(GameEventType t, std::function<void()> a) : type(t), action(a) {}
};

// Générateur d'événements aléatoires affectant l'état général du jeu
class RandomGameEventManager {
private:
    Math::Binomial        m_binomial;
    Math::Poisson         m_poisson;
    Math::UniformDiscrete m_uniformDiscrete;
    Math::ChiSquared      m_chiSquared;

    int m_turnCounter;

public:
    RandomGameEventManager();
    ~RandomGameEventManager() = default;

    void updateTurn() { m_turnCounter++; }
    int  getTurnCounter() const { return m_turnCounter; }

    // Événement: Révolution (Binomial)
    bool shouldRevolutionHappen();
    int  getRevolutionAffectedCount();

    // Événement: SwapReinePion (Poisson)
    int getEpidemicTransformCount();

    // Événement: CavalierSauvage (UniformDiscrete)
    int getRandomPieceSelector(int min, int max);

    // Événement: Daltonism (ChiSquared)
    double getDaltonismConfusion();
};

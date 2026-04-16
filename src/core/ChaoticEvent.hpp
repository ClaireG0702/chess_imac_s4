#pragma once

#include <functional>
#include "../math/Binomial.hpp"
#include "../math/ChiSquared.hpp"
#include "../math/Poisson.hpp"
#include "../math/UniformDiscrete.hpp"
#include "../math/Normal.hpp"
#include "../math/Exponential.hpp"
#include "../math/Cauchy.hpp"
#include "../math/Hypergeometric.hpp"

enum class ChaosEventType {
    // Game Events (Global Board Effects)
    Revolution,        // Binomial - Révolution politique
    EpidemicTransform, // Poisson - SwapReinePion
    CavalierSauvage,   // UniformDiscrete - Sélection aléatoire
    Daltonism,         // ChiSquared - Confusion daltonienne

    // Piece Events (Applied at Global Level)
    Dyscalculia,      // Normal - Erreur de calcul/mouvement
    Mutation,         // Exponential - Changement de type
    RuptureAttention, // Cauchy - Comportement chaotique
    Fuite,            // Hypergeometric - Sélection de cases espacées
};

// Unified Chaotic Event Manager
// Consolidates all 8 events (game + piece) into one global event system
// Each event triggers according to its associated random distribution
class RandomChaosEventManager {
private:
    // Game Event Distributions
    Math::Binomial        m_binomial;
    Math::Poisson         m_poisson;
    Math::UniformDiscrete m_uniformDiscrete;
    Math::ChiSquared      m_chiSquared;

    // Piece Event Distributions
    Math::Normal         m_normal;
    Math::Exponential    m_exponential;
    Math::Cauchy         m_cauchy;
    Math::Hypergeometric m_hypergeometric;

    int m_turnCounter;

public:
    RandomChaosEventManager();
    ~RandomChaosEventManager() = default;

    void updateTurn() { m_turnCounter++; }
    int  getTurnCounter() const { return m_turnCounter; }

    // ========== GAME EVENTS (Global Board Effects) ==========

    // Event 1: Revolution (Binomial)
    bool shouldRevolutionHappen();
    int  getRevolutionAffectedCount();

    // Event 2: EpidemicTransform (Poisson)
    int getEpidemicTransformCount();

    // Event 3: CavalierSauvage (UniformDiscrete)
    int getRandomPieceSelector(int min, int max);

    // Event 4: Daltonism (ChiSquared)
    double getDaltonismConfusion();

    // ========== PIECE EVENTS (Applied Globally) ==========

    // Event 5: Dyscalculia (Normal)
    double getDyscalculiaError();

    // Event 6: Mutation (Exponential)
    double getMutationChangeTime();

    // Event 7: RuptureAttention (Cauchy)
    double getRuptureAttentionRandomBehavior();

    // Event 8: Fuite (Hypergeometric)
    int getFuiteIsolatedCount();
};

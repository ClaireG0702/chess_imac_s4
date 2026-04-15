#include "GameEvent.hpp"

RandomGameEventManager::RandomGameEventManager()
    : m_binomial(16, 0.1), m_poisson(2.0), m_uniformDiscrete(0, 7), m_chiSquared(5.0), m_turnCounter(0) {}

// Révolution: Binomial(16, 0.1) - affects 0-16 pieces
bool RandomGameEventManager::shouldRevolutionHappen()
{
    m_binomial.setParameters(16, 0.1);
    int affected = m_binomial();
    return affected > 0;
}

int RandomGameEventManager::getRevolutionAffectedCount()
{
    m_binomial.setParameters(16, 0.1);
    return m_binomial();
}

// SwapReinePion: Poisson(2.0) - number of transformations
int RandomGameEventManager::getEpidemicTransformCount()
{
    m_poisson.setLambda(2.0);
    return m_poisson();
}

// CavalierSauvage: UniformDiscrete(0,7) - random piece selection
int RandomGameEventManager::getRandomPieceSelector(int min, int max)
{
    m_uniformDiscrete.setRange(min, max);
    return m_uniformDiscrete();
}

// Daltonism: ChiSquared(5) - confusion level (5 types of pieces)
double RandomGameEventManager::getDaltonismConfusion()
{
    m_chiSquared.setDegreesOfFreedom(5.0); // 5 types: Pawn, Knight, Bishop, Rook, Queen
    return m_chiSquared();
}

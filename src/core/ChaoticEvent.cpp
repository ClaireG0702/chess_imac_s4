#include "ChaoticEvent.hpp"

RandomChaosEventManager::RandomChaosEventManager()
    : m_binomial(16, 0.1), m_poisson(2.0), m_uniformDiscrete(0, 7), m_chiSquared(5.0),
      m_normal(0.0, 1.0), m_exponential(1.0), m_cauchy(0.0, 1.0), m_hypergeometric(64, 16, 8),
      m_turnCounter(0)
{
}

// Event 1: Revolution (Binomial)
bool RandomChaosEventManager::shouldRevolutionHappen()
{
    m_binomial.setParameters(16, 0.1);
    int affected = m_binomial();
    return affected > 0;
}

int RandomChaosEventManager::getRevolutionAffectedCount()
{
    m_binomial.setParameters(16, 0.1);
    return m_binomial();
}

// Event 2: EpidemicTransform (Poisson)
int RandomChaosEventManager::getEpidemicTransformCount()
{
    m_poisson.setLambda(2.0);
    return m_poisson();
}

// Event 3: CavalierSauvage (UniformDiscrete)
int RandomChaosEventManager::getRandomPieceSelector(int min, int max)
{
    m_uniformDiscrete.setRange(min, max);
    return m_uniformDiscrete();
}

// Event 4: Daltonism (ChiSquared)
double RandomChaosEventManager::getDaltonismConfusion()
{
    m_chiSquared.setDegreesOfFreedom(5.0);
    return m_chiSquared();
}

// Event 5: Dyscalculia (Normal)
double RandomChaosEventManager::getDyscalculiaError()
{
    m_normal.setParameters(0.0, 1.0);
    return m_normal();
}

// Event 6: Mutation (Exponential)
double RandomChaosEventManager::getMutationChangeTime()
{
    m_exponential.setLambda(1.0);
    return m_exponential();
}

// Event 7: RuptureAttention (Cauchy)
double RandomChaosEventManager::getRuptureAttentionRandomBehavior()
{
    m_cauchy.setParameters(0.0, 1.0);
    return m_cauchy();
}

// Event 8: Fuite (Hypergeometric)
int RandomChaosEventManager::getFuiteIsolatedCount()
{
    m_hypergeometric.setParameters(64, 16, 8);
    return m_hypergeometric();
}

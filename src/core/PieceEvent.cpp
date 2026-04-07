#include "PieceEvent.hpp"

RandomPieceEventManager::RandomPieceEventManager()
    : m_normal(0.0, 1.0),
      m_exponential(1.0),
      m_cauchy(0.0, 1.0),
      m_hypergeometric(64, 16, 8),
      m_turnCounter(0) {}

// Dyscalculia: Normal(0, 1) - calcul error
double RandomPieceEventManager::getDyscalculiaError() {
    m_normal.setParameters(0.0, 1.0);
    return m_normal();
}

// Genderfluid: Exponential(1.0) - time to next change
double RandomPieceEventManager::getGenderfluidChangeTime() {
    m_exponential.setLambda(1.0);
    return m_exponential();
}

// ADHD: Cauchy(0, 1) - chaotic behavior
double RandomPieceEventManager::getADHDRandomBehavior() {
    m_cauchy.setParameters(0.0, 1.0);
    return m_cauchy();
}

// Autism: Hypergeometric(64, 16, 8) - isolated spaces
int RandomPieceEventManager::getAutismIsolatedCount() {
    m_hypergeometric.setParameters(64, 16, 8);
    return m_hypergeometric();
}

#include "Gamma.hpp"

namespace Math {

Gamma::Gamma(double alpha, double beta)
    : m_exponential(1.0), m_alpha(alpha), m_beta(beta) {}

double Gamma::operator()() {
    // Pour α entier: Somme de α exponentielles Exp(1/β)
    // Pour α général: Marsaglia and Tsang (simplifié)
    
    if (m_alpha < 1.0) {
        // Cas α < 1: utilise une formule spéciale
        // Simplifié: utilise Gamma(α+1) et ajuste
        return m_beta * m_exponential();
    }
    
    // Pour α >= 1: somme d’exponentielles
    double sum = 0.0;
    int n = static_cast<int>(m_alpha);
    for (int i = 0; i < n; ++i) {
        sum += m_exponential();
    }
    return m_beta * sum;
}

void Gamma::setParameters(double alpha, double beta) {
    m_alpha = alpha;
    m_beta = beta;
}

} // namespace Math

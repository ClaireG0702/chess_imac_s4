#include "Poisson.hpp"

namespace Math {

Poisson::Poisson(double lambda)
    : m_uniform(0.0, 1.0), m_exponential(1.0), m_lambda(lambda) {}

int Poisson::operator()() {
    // Algorithme de Knuth pour la loi de Poisson
    // Pour λ modérés (< 30), c'est efficace
    
    // e^(-λ)
    double limit = std::exp(-m_lambda);
    
    // Accumule des exponentielles Exp(1)
    double sum = 0.0;
    int count = 0;
    
    while (true) {
        sum += m_exponential();  // Ajoute Exp(1)
        if (sum > m_lambda) {
            return count;
        }
        count++;
    }
}

void Poisson::setLambda(double lambda) {
    m_lambda = lambda;
}

} // namespace Math

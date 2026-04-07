#include "Normal.hpp"

namespace Math {

Normal::Normal(double mu, double sigma)
    : m_uniform(0.0, 1.0), m_mu(mu), m_sigma(sigma), m_has_spare(false), m_spare(0.0) {}

double Normal::operator()() {
    // Algorithme Box-Muller (Polar form)
    // Génère deux normales indépendantes N(0,1) en même temps
    
    if (m_has_spare) {
        m_has_spare = false;
        return m_mu + m_sigma * m_spare;
    }
    
    m_has_spare = true;
    
    double u, v, s;
    do {
        u = 2.0 * m_uniform() - 1.0;  // [-1, 1]
        v = 2.0 * m_uniform() - 1.0;  // [-1, 1]
        s = u * u + v * v;
    } while (s >= 1.0 || s == 0.0);  // Rejette si en dehors du cercle unitaire
    
    double multiply = std::sqrt(-2.0 * std::log(s) / s);
    
    m_spare = v * multiply;
    return m_mu + m_sigma * (u * multiply);
}

void Normal::setParameters(double mu, double sigma) {
    m_mu = mu;
    m_sigma = sigma;
    m_has_spare = false;  // Réinitialise l'état
}

} // namespace Math

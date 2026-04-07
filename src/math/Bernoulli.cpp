#include "Bernoulli.hpp"

namespace Math {

Bernoulli::Bernoulli(double p)
    : m_uniform(0.0, 1.0), m_p(p) {}

bool Bernoulli::operator()() {
    // Algorithme simple: U < p où U ~ U(0,1)
    return m_uniform() < m_p;
}

void Bernoulli::setProbability(double p) {
    m_p = p;
}

} // namespace Math

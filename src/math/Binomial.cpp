#include "Binomial.hpp"

namespace Math {

Binomial::Binomial(int n, double p)
    : m_bernoulli(p), m_n(n), m_p(p) {}

int Binomial::operator()() {
    // Pour petit n: somme de n Bernoullis indépendants
    // Pour grand n: on pourrait utiliser PTRS ou approximation normale
    int count = 0;
    for (int i = 0; i < m_n; ++i) {
        if (m_bernoulli()) {
            count++;
        }
    }
    return count;
}

void Binomial::setParameters(int n, double p) {
    m_n = n;
    m_p = p;
    m_bernoulli.setProbability(p);
}

} // namespace Math

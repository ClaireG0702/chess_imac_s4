#include "Hypergeometric.hpp"

namespace Math {

Hypergeometric::Hypergeometric(int N, int K, int n)
    : m_binomial(n, static_cast<double>(K) / N),
      m_N(N), m_K(K), m_n(n) {}

int Hypergeometric::operator()() {
    // Approximation par Binomial pour N grand (N >> n)
    // Plus exact qu'une vraie distribution hypergéométrique sans std::lib
    double p = static_cast<double>(m_K) / m_N;
    m_binomial.setParameters(m_n, p);
    return m_binomial();
}

void Hypergeometric::setParameters(int N, int K, int n) {
    m_N = N;
    m_K = K;
    m_n = n;
    double p = static_cast<double>(K) / N;
    m_binomial.setParameters(n, p);
}

} // namespace Math

#include "Cauchy.hpp"

#include <numbers>

namespace Math {

Cauchy::Cauchy(double a, double b)
    : m_uniform(0.0, 1.0), m_a(a), m_b(b) {}

double Cauchy::operator()() {
    // Algorithme: a + b * tan(π * (U - 0.5)) où U ~ U(0,1)
    constexpr double PI = std::numbers::pi;
    return m_a + m_b * std::tan(PI * (m_uniform() - 0.5));
}

void Cauchy::setParameters(double a, double b) {
    m_a = a;
    m_b = b;
}

} // namespace Math

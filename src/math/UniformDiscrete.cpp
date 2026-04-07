#include "UniformDiscrete.hpp"

namespace Math {

UniformDiscrete::UniformDiscrete(int a, int b)
    : m_uniform(0.0, 1.0), m_a(a), m_b(b) {}

int UniformDiscrete::operator()() {
    // U{a, b}: floor(a + (b-a+1)*U) où U ~ U(0,1)
    return m_a + static_cast<int>((m_b - m_a + 1) * m_uniform());
}

void UniformDiscrete::setRange(int a, int b) {
    m_a = a;
    m_b = b;
}

} // namespace Math

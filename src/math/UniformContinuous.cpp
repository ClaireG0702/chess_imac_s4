#include "UniformContinuous.hpp"

namespace Math {

UniformContinuous::UniformContinuous(double a, double b)
    : m_gen(m_rd()), m_a(a), m_b(b) {}

double UniformContinuous::raw() {
    // Génère un nombre dans [0, 1) à partir de MT19937
    // MT19937 produit des entiers dans [0, 2^32)
    uint32_t value = m_gen();
    return static_cast<double>(value) / 4294967296.0;  // 2^32
}

double UniformContinuous::operator()() {
    // Transformation linéaire: U(a,b) = a + (b-a) * U(0,1)
    return m_a + (m_b - m_a) * raw();
}

void UniformContinuous::setRange(double a, double b) {
    m_a = a;
    m_b = b;
}

} // namespace Math

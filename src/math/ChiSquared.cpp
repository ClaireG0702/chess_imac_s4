#define _USE_MATH_DEFINES
#include "ChiSquared.hpp"
#include <cmath>

namespace Math {

ChiSquared::ChiSquared(double k) : m_degreesOfFreedom(k) {}

double ChiSquared::operator()()
{
    // χ²(k) = Z₁² + Z₂² + ... + Zₖ² où Zᵢ ~ N(0, 1)
    // Utiliser la Box-Muller pour générer deux normales standard à partir de deux uniformes
    double chi_squared_sum = 0.0;
    int    k               = static_cast<int>(m_degreesOfFreedom);

    for (int i = 0; i < k; ++i)
    {
        // Générer deux uniformes U(0,1)
        double u1 = m_uniform();
        double u2 = m_uniform();

        // Box-Muller: transformer en deux normales standard
        // Z = sqrt(-2 * ln(U1)) * cos(2π * U2)
        double z1 = std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * M_PI * u2);

        // Ajouter z1² à la somme
        chi_squared_sum += z1 * z1;
    }

    return chi_squared_sum;
}

} // namespace Math

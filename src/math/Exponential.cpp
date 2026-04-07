#include "Exponential.hpp"

namespace Math {

Exponential::Exponential(double lambda)
    : m_uniform(0.0, 1.0), m_lambda(lambda) {}

double Exponential::operator()() {
    // Algorithme: Inverse transform sampling
    // X = -ln(U) / λ où U ~ U(0,1)
    // Note: On fait U = 1 - U pour éviter ln(0)
    double u = m_uniform();
    return -std::log(1.0 - u) / m_lambda;
}

void Exponential::setLambda(double lambda) {
    m_lambda = lambda;
}

} // namespace Math

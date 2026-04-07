#pragma once

#include "UniformContinuous.hpp"

namespace Math {

/**
 * Loi du Chi-Deux χ²(k)
 * 
 * Loi continue à densité obtenue comme somme de k variables normales centrées réduites au carré.
 * χ²(k) = Z₁² + Z₂² + ... + Zₖ² où Zᵢ ~ N(0, 1)
 * 
 * Utilisée pour modéliser les écarts au carré, notamment la confusion daltonienne.
 * Exemple: Niveau de confusion couleur dans l'événement Daltonien
 */
class ChiSquared {
private:
    UniformContinuous m_uniform;
    double m_degreesOfFreedom;

public:
    /**
     * Constructeur
     * @param k degrés de liberté (nombre de variables normales)
     */
    explicit ChiSquared(double k = 2.0);

    /**
     * Génère une variable aléatoire suivant χ²(k)
     */
    double operator()();

    /**
     * Définir les degrés de liberté
     */
    void setDegreesOfFreedom(double k) { m_degreesOfFreedom = k; }

    /**
     * Obtenir les degrés de liberté
     */
    double getDegreesOfFreedom() const { return m_degreesOfFreedom; }

    /**
     * Espérance E[χ²(k)] = k
     */
    double getMean() const { return m_degreesOfFreedom; }

    /**
     * Variance Var[χ²(k)] = 2k
     */
    double getVariance() const { return 2.0 * m_degreesOfFreedom; }
};

} // namespace Math

#pragma once

#include "Exponential.hpp"
#include <cmath>

namespace Math {

/**
 * Loi Gamma Γ(α, β) ou Loi d'Erlang
 * 
 * Généralisation de la loi exponentielle.
 * Utilisée pour modéliser les temps d'attente cumulés.
 * 
 * Paramètres:
 * - α (alpha): paramètre de forme (> 0)
 * - β (beta): paramètre d'échelle (> 0)
 * 
 * Caractéristiques:
 * - Domaine: [0, +∞)
 * - Espérance: α * β
 * - Variance: α * β²
 * - Si α = 1: loi exponentielle
 * - Si β = 1/λ et α = n: loi d'Erlang
 * 
 * Exemple: Fusion - Temps nécessaire avant fusion de deux pions
 */
class Gamma {
private:
    Exponential m_exponential;
    double m_alpha;
    double m_beta;

public:
    /**
     * Constructeur
     * @param alpha paramètre de forme (> 0)
     * @param beta paramètre d'échelle (> 0)
     */
    Gamma(double alpha = 1.0, double beta = 1.0);

    /**
     * Génère une valeur selon la loi Gamma
     * @return nombre réel positif
     */
    double operator()();

    /**
     * Change les paramètres
     * @param alpha nouveau paramètre de forme
     * @param beta nouvel paramètre d'échelle
     */
    void setParameters(double alpha, double beta);
};

} // namespace Math

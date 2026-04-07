#pragma once

#include "UniformContinuous.hpp"
#include <cmath>

namespace Math {

/**
 * Loi normale / Loi de Gauss N(μ, σ²)
 * 
 * La distribution la plus importante en probabilité.
 * Implémentée via l'algorithme de Box-Muller (réduction de dimension).
 * 
 * Paramètres:
 * - μ (mu): espérance (moyenne)
 * - σ (sigma): écart-type
 * 
 * Caractéristiques:
 * - Domaine: tous les réels (-∞, +∞)
 * - Espérance: μ
 * - Variance: σ²
 * - Symétrique autour de μ
 * 
 * Exemple: Dyscalculie - Erreur de mouvement autour de la distance prévue
 * 
 * Algorithme: Box-Muller (polar form)
 */
class Normal {
private:
    UniformContinuous m_uniform;
    double m_mu;
    double m_sigma;
    bool m_has_spare;
    double m_spare;

public:
    /**
     * Constructeur
     * @param mu espérance (moyenne) - par défaut 0
     * @param sigma écart-type - par défaut 1
     */
    Normal(double mu = 0.0, double sigma = 1.0);

    /**
     * Génère une valeur selon la loi normale N(μ, σ²)
     * via l'algorithme de Box-Muller
     * @return nombre réel distribué selon N(μ, σ²)
     */
    double operator()();

    /**
     * Change les paramètres
     * @param mu nouvelle espérance
     * @param sigma nouvel écart-type
     */
    void setParameters(double mu, double sigma);
};

} // namespace Math

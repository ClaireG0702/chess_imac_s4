#pragma once

#include <cmath>
#include "UniformContinuous.hpp"


namespace Math {

/**
 * Loi exponentielle Exp(λ)
 *
 * Modélise le temps d'attente entre événements d'un processus Poisson.
 * Implémentée via inverse transform sampling: X = -ln(U) / λ
 *
 * Paramètres:
 * - λ (lambda): taux (intensité > 0)
 *
 * Caractéristiques:
 * - Domaine: [0, +∞)
 * - Espérance: 1/λ
 * - Variance: 1/λ²
 * - Sans mémoire: P(X > s+t | X > s) = P(X > t)
 *
 * Exemple: Mutation - Temps d'attente avant changement de type de pièce
 *
 * Algorithme: Inverse transform sampling
 */
class Exponential {
private:
    UniformContinuous m_uniform;
    double            m_lambda;

public:
    /**
     * Constructeur
     * @param lambda taux (> 0) - par défaut 1
     */
    Exponential(double lambda = 1.0);

    /**
     * Génère une valeur selon la loi exponentielle
     * via X = -ln(U) / λ où U ~ U(0,1)
     * @return nombre réel positif
     */
    double operator()();

    /**
     * Change le paramètre lambda
     * @param lambda nouveau taux
     */
    void setLambda(double lambda);
};

} // namespace Math

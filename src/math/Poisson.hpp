#pragma once

#include <cmath>
#include "Exponential.hpp"
#include "UniformContinuous.hpp"

namespace Math {

/**
 * Loi de Poisson P(λ)
 *
 * Compte le nombre d'événements se produisant dans un intervalle de temps/espace.
 * Implémentée via l'algorithme de Knuth (pour λ modéré):
 * - Génère des exponentielles et les accumule jusqu'à dépasser λ
 *
 * Paramètres:
 * - λ (lambda): taux moyen d'occurrences (> 0)
 *
 * Caractéristiques:
 * - Domaine: {0, 1, 2, 3, ...}
 * - Espérance: λ
 * - Variance: λ
 * - Point clé: E[X] = Var(X) = λ
 *
 * Exemple: SwapReinePion - Nombre de transformations pion↔dame par tour
 *
 * Algorithme: Knuth's algorithm (pour λ < 30)
 */
class Poisson {
private:
    UniformContinuous m_uniform;
    Exponential       m_exponential;
    double            m_lambda;

public:
    /**
     * Constructeur
     * @param lambda taux moyen (> 0)
     */
    Poisson(double lambda = 1.0);

    /**
     * Génère une valeur selon la loi de Poisson
     * @return nombre entier d'événements
     */
    int operator()();

    /**
     * Change le paramètre lambda
     * @param lambda nouveau taux moyen
     */
    void setLambda(double lambda);
};

} // namespace Math

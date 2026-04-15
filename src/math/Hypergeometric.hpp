#pragma once

#include <cmath>
#include "Binomial.hpp"


namespace Math {

/**
 * Loi hypergéométrique H(N, K, n)
 *
 * Modélise le tirage sans remise d'un ensemble fini.
 * Nombre de succès dans un échantillon sans remise.
 *
 * Paramètres:
 * - N: taille totale de la population
 * - K: nombre de succès dans la population
 * - n: taille de l'échantillon tiré
 *
 * Caractéristiques:
 * - Domaine: max(0, n-N+K) à min(n, K)
 * - Espérance: n * K / N
 * - Variance: n * K/N * (N-K)/N * (N-n)/(N-1)
 * - Approximée par binomiale si N >> n
 *
 * Exemple: Fuite - Sélectionner des cases isolées sans remise du plateau
 *
 * Note: Implémentation manuelle car hypergeometric_distribution n'existe pas en C++20
 */
class Hypergeometric {
private:
    Binomial m_binomial;
    int      m_N; // total population
    int      m_K; // number of success states in population
    int      m_n; // number of draws

public:
    /**
     * Constructeur
     * @param N taille totale de la population
     * @param K nombre de succès dans la population
     * @param n taille de l'échantillon
     */
    Hypergeometric(int N = 100, int K = 50, int n = 10);

    /**
     * Génère une valeur selon la loi hypergéométrique (approximation par Binomiale)
     * @return nombre de succès dans l'échantillon
     */
    int operator()();

    /**
     * Change les paramètres
     * @param N nouvelle taille de population
     * @param K nouveau nombre de succès
     * @param n nouvelle taille d'échantillon
     */
    void setParameters(int N, int K, int n);
};

} // namespace Math

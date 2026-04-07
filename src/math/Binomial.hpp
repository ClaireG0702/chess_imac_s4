#pragma once

#include "Bernoulli.hpp"
#include <cmath>

namespace Math {

/**
 * Loi binomiale B(n, p)
 * 
 * Utilisée pour modéliser le nombre de succès dans n essais indépendants,
 * chacun ayant une probabilité p de succès.
 * 
 * Exemple: Nombre de pièces affectées par un événement parmi n pièces,
 * avec une probabilité p pour chaque pièce.
 */
class Binomial {
private:
    Bernoulli m_bernoulli;
    int m_n;
    double m_p;

public:
    /**
     * Constructeur
     * @param n nombre d'essais
     * @param p probabilité de succès (entre 0 et 1)
     */
    Binomial(int n = 1, double p = 0.5);

    /**
     * Génère une valeur selon la loi binomiale B(n, p)
     * @return nombre de succès
     */
    int operator()();

    /**
     * Change les paramètres de la loi binomiale
     * @param n nouveau nombre d'essais
     * @param p nouvelle probabilité de succès
     */
    void setParameters(int n, double p);
};

} // namespace Math

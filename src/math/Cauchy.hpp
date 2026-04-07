#pragma once

#include "UniformContinuous.hpp"
#include <cmath>

namespace Math {

/**
 * Loi de Cauchy (Cauchy Distribution)
 * 
 * Loi pathologique sans espérance ni variance finie.
 * Exemple classique d'une loi avec comportement chaotique.
 * 
 * Paramètres:
 * - a: localisation (médiane)
 * - b: échelle (> 0)
 * 
 * Caractéristiques:
 * - Domaine: (-∞, +∞)
 * - Espérance: non définie
 * - Variance: infinie
 * - Queues très épaisses → valeurs extrêmes fréquentes
 * 
 * Exemple: TDAH - Comportement très chaotique et imprévisible
 */
class Cauchy {
private:
    UniformContinuous m_uniform;
    double m_a;
    double m_b;

public:
    /**
     * Constructeur
     * @param a localisation - par défaut 0
     * @param b échelle - par défaut 1
     */
    Cauchy(double a = 0.0, double b = 1.0);

    /**
     * Génère une valeur selon la loi de Cauchy
     * @return nombre réel (peut être très extrême)
     */
    double operator()();

    /**
     * Change les paramètres
     * @param a nouvelle localisation
     * @param b nouvel échelle
     */
    void setParameters(double a, double b);
};

} // namespace Math

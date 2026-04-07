#pragma once

#include "UniformContinuous.hpp"
#include <cmath>

namespace Math {

/**
 * Loi uniforme discrète U{a, b}
 * 
 * Génère des entiers uniformément distribués entre a et b inclus.
 * Chaque valeur a la même probabilité d'être sélectionnée.
 * 
 * Exemple: Choisir aléatoirement une case du plateau [0, 63].
 */
class UniformDiscrete {
private:
    UniformContinuous m_uniform;
    int m_a;
    int m_b;

public:
    /**
     * Constructeur
     * @param a borne inférieure (inclusive)
     * @param b borne supérieure (inclusive)
     */
    UniformDiscrete(int a = 0, int b = 1);

    /**
     * Génère une valeur selon la loi uniforme discrète
     * @return entier entre a et b
     */
    int operator()();

    /**
     * Change les bornes
     * @param a nouvelle borne inférieure
     * @param b nouvelle borne supérieure
     */
    void setRange(int a, int b);
};

} // namespace Math

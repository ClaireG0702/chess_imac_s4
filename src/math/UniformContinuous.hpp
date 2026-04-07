#pragma once

#include <random>
#include <cmath>

namespace Math {

/**
 * Loi uniforme continue U[a, b)
 * 
 * Génère des nombres réels uniformément distribués entre a et b.
 * Utilise std::mt19937 mais réimplémente la génération manuellement.
 * 
 * Générateur base pour tous les autres algorithmes de simulation.
 */
class UniformContinuous {
private:
    std::random_device m_rd;
    std::mt19937 m_gen;
    double m_a;
    double m_b;

public:
    /**
     * Constructeur
     * @param a borne inférieure (par défaut 0.0)
     * @param b borne supérieure (par défaut 1.0)
     */
    UniformContinuous(double a = 0.0, double b = 1.0);

    /**
     * Génère une valeur selon la loi uniforme continue U(a,b)
     * en transformant une sortie [0, 1) de mt19937
     * @return nombre réel dans [a, b)
     */
    double operator()();

    /**
     * Change les bornes
     * @param a nouvelle borne inférieure
     * @param b nouvelle borne supérieure
     */
    void setRange(double a, double b);
    
    /**
     * Accès direct au générateur MT19937 pour être utilisé par d'autres lois
     * @return valeur aléatoire dans [0, 1)
     */
    double raw();
};

} // namespace Math

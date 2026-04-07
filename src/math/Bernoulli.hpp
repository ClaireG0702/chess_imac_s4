#pragma once

#include "UniformContinuous.hpp"

namespace Math {

/**
 * Loi de Bernoulli B(p)
 * 
 * Cas spécial de la loi binomiale avec n=1.
 * Retourne true avec probabilité p, false avec probabilité 1-p.
 * 
 * Implémentée directement: retourner U < p
 * 
 * Paramètres:
 * - p: probabilité de succès (entre 0 et 1)
 * 
 * Caractéristiques:
 * - Espérance: p
 * - Variance: p(1-p)
 * 
 * Exemple: Déterminer si un événement rare se produit avec probabilité p.
 * 
 * Algorithme: U < p où U ~ U(0,1)
 */
class Bernoulli {
private:
    UniformContinuous m_uniform;
    double m_p;

public:
    /**
     * Constructeur
     * @param p probabilité de succès (entre 0 et 1)
     */
    Bernoulli(double p = 0.5);

    /**
     * Génère une valeur selon la loi de Bernoulli
     * @return true avec probabilité p, false sinon
     */
    bool operator()();

    /**
     * Change la probabilité
     * @param p nouvelle probabilité de succès
     */
    void setProbability(double p);
};

} // namespace Math

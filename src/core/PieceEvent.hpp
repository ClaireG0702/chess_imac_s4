#pragma once

#include <functional>
#include "../math/Cauchy.hpp"
#include "../math/Exponential.hpp"
#include "../math/Hypergeometric.hpp"
#include "../math/Normal.hpp"

enum class PieceEventType {
    Dyscalculia,      // Normal - Erreur de calcul/mouvement
    Mutation,         // Exponential - Changement de type de pièce
    RuptureAttention, // Cauchy - Comportement chaotique (heavy tails)
    Fuite,            // Hypergeometric - Sélection de cases espacées
};

class PieceEvent {
public:
    PieceEventType        type;
    std::function<void()> action;

    PieceEvent(PieceEventType t, std::function<void()> a) : type(t), action(a) {}
};

// Générateur d'événements aléatoires affectant les pièces
class RandomPieceEventManager {
private:
    Math::Normal         m_normal;
    Math::Exponential    m_exponential;
    Math::Cauchy         m_cauchy;
    Math::Hypergeometric m_hypergeometric;

    int m_turnCounter;

public:
    RandomPieceEventManager();
    ~RandomPieceEventManager() = default;

    void updateTurn() { m_turnCounter++; }
    int  getTurnCounter() const { return m_turnCounter; }

    // Événement: Dyscalculia (Normal)
    double getDyscalculiaError();

    // Événement: Mutation (Exponential)
    double getMutationChangeTime();

    // Événement: RuptureAttention (Cauchy)
    double getRuptureAttentionRandomBehavior();

    // Événement: Fuite (Hypergeometric)
    int getFuiteIsolatedCount();
};

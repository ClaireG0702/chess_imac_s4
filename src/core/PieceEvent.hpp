#pragma once

#include <functional>
#include "../math/Normal.hpp"
#include "../math/Exponential.hpp"
#include "../math/Cauchy.hpp"
#include "../math/Hypergeometric.hpp"

enum class PieceEventType {
    Dyscalculia,   // Normal - Erreur de calcul/mouvement
    Genderfluid,   // Exponential - Changement de type de pièce
    ADHD,          // Cauchy - Comportement chaotique (heavy tails)
    Autism,        // Hypergeometric - Sélection de cases espacées
};

class PieceEvent {
public:
    PieceEventType type;
    std::function<void()> action;

    PieceEvent(PieceEventType t, std::function<void()> a) : type(t), action(a) {}
};

// Générateur d'événements aléatoires affectant les pièces
class RandomPieceEventManager {
private:
    Math::Normal m_normal;
    Math::Exponential m_exponential;
    Math::Cauchy m_cauchy;
    Math::Hypergeometric m_hypergeometric;
    
    int m_turnCounter;

public:
    RandomPieceEventManager();
    ~RandomPieceEventManager() = default;

    void updateTurn() { m_turnCounter++; }
    int getTurnCounter() const { return m_turnCounter; }
    
    // Événement: Dyscalculia (Normal)
    double getDyscalculiaError();
    
    // Événement: Genderfluid (Exponential)
    double getGenderfluidChangeTime();
    
    // Événement: ADHD (Cauchy)
    double getADHDRandomBehavior();
    
    // Événement: Autism (Hypergeometric)
    int getAutismIsolatedCount();
};

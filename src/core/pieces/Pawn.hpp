#pragma once
#include "../Piece.hpp"
#include <vector>
#include <utility>

class Pawn : public Piece {
    public:
        Pawn(int x, int y, Color color);
        virtual ~Pawn() = default;
        
        // Retourne les positions possibles pour les mouvements du pion
        std::vector<std::pair<int, int>> getPossibleMoves() const;
        
    private:
        bool hasMovedFromStart = false;
};
#include "../Piece.hpp"
#include <vector>
#include <utility>

class Bishop : public Piece {
    public:
        Bishop(int x, int y, Color color);
        virtual ~Bishop() = default;
        
        // Retourne les positions possibles pour les mouvements du fou
        std::vector<std::pair<int, int>> getPossibleMoves() const;
};
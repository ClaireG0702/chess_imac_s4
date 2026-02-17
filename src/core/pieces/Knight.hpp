#include "../Piece.hpp"
#include <vector>
#include <utility>

class Knight : public Piece {
    public:
        Knight(int x, int y, Color color);
        virtual ~Knight() = default;
        
        // Retourne les positions possibles pour les mouvements du fou
        std::vector<std::pair<int, int>> getPossibleMoves() const;
};
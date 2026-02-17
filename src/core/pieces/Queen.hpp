#include "../Piece.hpp"
#include <vector>
#include <utility>

class Queen : public Piece {
    public:
        Queen(int x, int y, Color color);
        virtual ~Queen() = default;
        
        // Retourne les positions possibles pour les mouvements du fou
        std::vector<std::pair<int, int>> getPossibleMoves() const;
};
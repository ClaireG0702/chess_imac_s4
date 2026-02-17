#include "../Piece.hpp"
#include <vector>
#include <utility>

class King : public Piece {
    public:
        King(int x, int y, Color color);
        virtual ~King() = default;
        
        // Retourne les positions possibles pour les mouvements du fou
        std::vector<std::pair<int, int>> getPossibleMoves() const;
};
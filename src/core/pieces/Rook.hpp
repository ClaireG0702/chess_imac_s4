#include "../Piece.hpp"
#include <vector>
#include <utility>

class Rook : public Piece {
    public:
        Rook(int x, int y, Color color);
        virtual ~Rook() = default;
        
        // Retourne les positions possibles pour les mouvements de la tour
        std::vector<std::pair<int, int>> getPossibleMoves() const;
};
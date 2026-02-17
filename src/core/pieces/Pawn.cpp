#include "Pawn.hpp"
#include <utility>

Pawn::Pawn(int x, int y, Color color)
    : Piece(x, y, color)
{
}

std::vector<std::pair<int, int>> Pawn::getPossibleMoves() const {
    std::vector<std::pair<int, int>> moves;
    
    if(getColor() == Color::White) {
        // Move forward
        moves.emplace_back(getX(), getY() + 1);
        // Initial double move
        if (!hasMovedFromStart) {
            moves.emplace_back(getX(), getY() + 2);
        }
        // Capture moves
        moves.emplace_back(getX() - 1, getY() + 1);
        moves.emplace_back(getX() + 1, getY() + 1);
    } else {
        // Move forward
        moves.emplace_back(getX(), getY() - 1);
        // Initial double move
        if (!hasMovedFromStart) {
            moves.emplace_back(getX(), getY() - 2);
        }
        // Capture moves
        moves.emplace_back(getX() - 1, getY() - 1);
        moves.emplace_back(getX() + 1, getY() - 1);
    }
    
    return moves;
}

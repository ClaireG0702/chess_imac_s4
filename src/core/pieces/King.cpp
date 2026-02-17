#include "King.hpp"

King::King(int x, int y, Color color)
    : Piece(x, y, color)
{
}

std::vector<std::pair<int, int>> King::getPossibleMoves() const {
    std::vector<std::pair<int, int>> moves;

    return moves;
}
#include "Bishop.hpp"

Bishop::Bishop(int x, int y, Color color)
    : Piece(x, y, color)
{
}

std::vector<std::pair<int, int>> Bishop::getPossibleMoves() const {
    std::vector<std::pair<int, int>> moves;

    return moves;
}
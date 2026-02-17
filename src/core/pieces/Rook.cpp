#include "Rook.hpp"

Rook::Rook(int x, int y, Color color)
    : Piece(x, y, color)
{
}

std::vector<std::pair<int, int>> Rook::getPossibleMoves() const {
    std::vector<std::pair<int, int>> moves;

    return moves;
}
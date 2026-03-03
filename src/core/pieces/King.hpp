#pragma once
#include "../Piece.hpp"

class King : public Piece {
    public:
        explicit King(Color color);

        bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const override;
        std::vector<std::pair<int, int>> getPossibleMoves(int row, int col, const Board& board) const override;
};
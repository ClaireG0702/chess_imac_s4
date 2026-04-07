#pragma once
#include "../Piece.hpp"

class Knight : public Piece {
    public:
        explicit Knight(Color color);

        bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const override;
        std::vector<std::pair<int, int>> getPossibleMoves(int row, int col, const Board& board) const override;
        std::unique_ptr<Piece> clone() const override;
};
#pragma once
#include "Piece.hpp"
#include <array>
#include <memory>

class Board {
    public:
        Board();
        ~Board() = default;
        Board(const Board&) = delete;
        Board& operator=(const Board&) = delete;
        Board(Board&&) = delete;
        Board& operator=(Board&&) = delete;

        void initialize();
        Piece *getPieceAt(int row, int col) const;
        bool setPieceAt(int row, int col, std::unique_ptr<Piece> piece);
        bool movePiece(int fromRow, int fromCol, int toRow, int toCol);
        bool isCellEmpty(int row, int col) const;
        bool isValidPosition(int row, int col) const;
        void clear();

    private:
        std::array<std::array<std::unique_ptr<Piece>, 8>, 8> board; // 8x8 grid of pointers to pieces (nullptr if no piece on the square)
};
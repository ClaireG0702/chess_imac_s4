#pragma once
#include "Piece.hpp"
#include <array>
#include <memory>
#include <vector>
#include <random>

constexpr int BOARD_SIZE = 8;

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
        std::array<std::array<std::unique_ptr<Piece>, BOARD_SIZE>, BOARD_SIZE> board; // Board grid of pointers to pieces
        
        void assignCharacteristicsToBoard();
        std::vector<std::pair<int, int>> getAllPiecesExceptKings() const;
        void assignCharacteristic(const std::vector<std::pair<int, int>>& pieces, 
                                 PieceCharacteristic characteristic, 
                                 int count, 
                                 std::mt19937& gen);
};
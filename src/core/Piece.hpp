#pragma once
#include <cstdint>
#include <vector>

enum class Color : std::uint8_t { 
    White,
    Black 
};

enum class PieceType : std::uint8_t {
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King
};

class Board; // Forward declaration of Board class

class Piece {
    public:
        Piece(Color color, PieceType type);
        virtual ~Piece() = default;
        Piece(const Piece&) = delete;
        Piece& operator=(const Piece&) = delete;
        Piece(Piece&&) = delete;
        Piece& operator=(Piece&&) = delete;

        Color getColor() const { return m_color; }
        PieceType getType() const { return m_type; }
        bool hasMoved() const { return m_hasMoved; }
        void setMoved() { m_hasMoved = true; }
        void toString() const;

        virtual bool isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const = 0; // Pure virtual function to be implemented by derived classes
        virtual std::vector<std::pair<int, int>> getPossibleMoves(int row, int col, const Board& board) const = 0; // Pure virtual function to be implemented by derived classes

    protected:
        Color m_color;
        PieceType m_type;
        bool m_hasMoved;

        bool isPathClear(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const;
};
#pragma once
#include <cstdint>
#include <memory>
#include <vector>

enum class Color : std::uint8_t {
    White,
    Black,
    Gray // For Daltonism event
};

enum class PieceType : std::uint8_t {
    Pawn,
    Rook,
    Knight,
    Bishop,
    Queen,
    King
};

enum class PieceCharacteristic : std::uint8_t {
    Fuite            = 1 << 0, // bit 0 = 1
    RuptureAttention = 1 << 1, // bit 1 = 2
    Dyscalculia      = 1 << 2, // bit 2 = 4
    Mutation         = 1 << 3  // bit 3 = 8
};

class Board; // Forward declaration of Board class

class Piece {
public:
    Piece(Color color, PieceType type);
    Piece(Color color, PieceType type, std::uint8_t characteristics);
    virtual ~Piece()               = default;
    Piece(const Piece&)            = delete;
    Piece& operator=(const Piece&) = delete;
    Piece(Piece&&)                 = delete;
    Piece& operator=(Piece&&)      = delete;

    Color     getColor() const { return m_color; }
    void      setColor(Color color) { m_color = color; }
    PieceType getType() const { return m_type; }

    // Characteristics methods (bitset-based)
    bool hasCharacteristic(PieceCharacteristic c) const
    {
        return m_characteristics & static_cast<std::uint8_t>(c);
    }
    void addCharacteristic(PieceCharacteristic c)
    {
        m_characteristics |= static_cast<std::uint8_t>(c);
    }
    void removeCharacteristic(PieceCharacteristic c)
    {
        m_characteristics &= ~static_cast<std::uint8_t>(c);
    }
    std::uint8_t getCharacteristics() const { return m_characteristics; }
    void         setCharacteristics(std::uint8_t characteristics) { m_characteristics = characteristics; }

    bool hasMoved() const { return m_hasMoved; }
    void setMoved() { m_hasMoved = true; }
    void toString() const;

    virtual bool                             isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const = 0; // Pure virtual function to be implemented by derived classes
    virtual std::vector<std::pair<int, int>> getPossibleMoves(int row, int col, const Board& board) const                          = 0; // Pure virtual function to be implemented by derived classes
    virtual std::unique_ptr<Piece>           clone() const                                                                         = 0; // Clone method for deep copying

protected:
    Color        m_color;
    PieceType    m_type;
    std::uint8_t m_characteristics; // Bitset for multiple characteristics
    bool         m_hasMoved;

    bool isPathClear(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const;
};
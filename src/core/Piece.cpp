#include "Piece.hpp"
#include "Board.hpp"
#include <iostream>

Piece::Piece(Color color, PieceType type) : m_color(color), m_type(type), m_characteristics(0), m_hasMoved(false) {}

Piece::Piece(Color color, PieceType type, std::uint8_t characteristics) : m_color(color), m_type(type), m_characteristics(characteristics), m_hasMoved(false) {}

bool Piece::isPathClear(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const {
    int rowDir = 0;
    if (toRow > fromRow) {
        rowDir = 1;
    } else if (toRow < fromRow) {
        rowDir = -1;
    }
    
    int colDir = 0;
    if (toCol > fromCol) {
        colDir = 1;
    } else if (toCol < fromCol) {
        colDir = -1;
    }
    
    int currentRow = fromRow + rowDir;
    int currentCol = fromCol + colDir;
    
    while (currentRow != toRow || currentCol != toCol) {
        if (!board.isCellEmpty(currentRow, currentCol)) {
            return false;
        }
        currentRow += rowDir;
        currentCol += colDir;
    }
    
    return true;
}

void Piece::toString() const {
    char symbol = ' ';
    switch (m_type) {
        case PieceType::Pawn:   symbol = 'P'; break;
        case PieceType::Rook:   symbol = 'R'; break;
        case PieceType::Knight: symbol = 'N'; break;
        case PieceType::Bishop: symbol = 'B'; break;
        case PieceType::Queen:  symbol = 'Q'; break;
        case PieceType::King:   symbol = 'K'; break;
    }
    std::cout << "Piece: " << symbol << " Color: " << (m_color == Color::White ? "White" : "Black") << '\n';
}
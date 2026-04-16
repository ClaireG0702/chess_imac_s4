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

std::vector<std::pair<int, int>> Piece::getDirectionalMoves(
    int row, int col, const Board& board,
    const std::vector<std::array<int, 2>>& directions) const {
    std::vector<std::pair<int, int>> moves;
    
    for (const auto& dir : directions) {
        int newRow = row + dir[0];
        int newCol = col + dir[1];
        
        while (board.isValidPosition(newRow, newCol)) {
            if (!canCaptureOrEmpty(newRow, newCol, board)) {
                break;  // Blocked by own piece
            }
            moves.push_back({newRow, newCol});
            
            Piece* piece = board.getPieceAt(newRow, newCol);
            if (piece && piece->getColor() != m_color) {
                break;  // Captured opponent, can't go further
            }
            
            newRow += dir[0];
            newCol += dir[1];
        }
    }
    
    return moves;
}

bool Piece::canCaptureOrEmpty(int row, int col, const Board& board) const {
    Piece* targetPiece = board.getPieceAt(row, col);
    return !targetPiece || targetPiece->getColor() != m_color;
}
#include "Pawn.hpp"
#include "../Board.hpp"
#include <cmath>


Pawn::Pawn(Color color) : Piece(color, PieceType::Pawn) {}

bool Pawn::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const {
    int direction = (m_color == Color::White) ? 1 : -1;
    int rowDiff = toRow - fromRow;
    int colDiff = std::abs(toCol - fromCol);
    
    // Avancer d'une case
    if (colDiff == 0 && rowDiff == direction && board.isCellEmpty(toRow, toCol)) {
        return true;
    }
    
    // Avancer de deux cases depuis la position initiale
    int startRow = (m_color == Color::White) ? 1 : 6;
    if (!m_hasMoved && colDiff == 0 && rowDiff == 2 * direction && 
        fromRow == startRow && board.isCellEmpty(toRow, toCol) &&
        board.isCellEmpty(fromRow + direction, fromCol)) {
        return true;
    }
    
    // Capture en diagonale
    if (colDiff == 1 && rowDiff == direction) {
        Piece* targetPiece = board.getPieceAt(toRow, toCol);
        if (targetPiece && targetPiece->getColor() != m_color) {
            return true;
        }
    }
    
    return false;
}

std::vector<std::pair<int, int>> Pawn::getPossibleMoves(int row, int col, const Board& board) const {
    std::vector<std::pair<int, int>> moves;
    int direction = (m_color == Color::White) ? 1 : -1;
    
    // Avancer d'une case
    if (board.isValidPosition(row + direction, col) && 
        board.isCellEmpty(row + direction, col)) {
        moves.push_back({row + direction, col});
        
        // Avancer de deux cases
        int startRow = (m_color == Color::White) ? 1 : 6;
        if (!m_hasMoved && row == startRow && 
            board.isCellEmpty(row + 2 * direction, col)) {
            moves.push_back({row + 2 * direction, col});
        }
    }
    
    // Captures diagonales
    for (int dc = -1; dc <= 1; dc += 2) {
        int newRow = row + direction;
        int newCol = col + dc;
        if (board.isValidPosition(newRow, newCol)) {
            Piece* piece = board.getPieceAt(newRow, newCol);
            if (piece && piece->getColor() != m_color) {
                moves.push_back({newRow, newCol});
            }
        }
    }
    
    return moves;
}

std::unique_ptr<Piece> Pawn::clone() const {
    auto cloned = std::make_unique<Pawn>(m_color);
    cloned->m_hasMoved = m_hasMoved;
    return cloned;
}

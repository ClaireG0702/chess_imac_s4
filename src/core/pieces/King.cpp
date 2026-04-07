#include "King.hpp"
#include "../Board.hpp"
#include <cmath>
#include <vector>
#include <array>

King::King(Color color) : Piece(color, PieceType::King) {}

bool King::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const {
    int rowDiff = std::abs(toRow - fromRow);
    int colDiff = std::abs(toCol - fromCol);
    
    // Le roi se déplace d'une case dans n'importe quelle direction
    if (rowDiff > 1 || colDiff > 1) {
        return false;
    }
    
    // Vérifier la case de destination
    Piece* targetPiece = board.getPieceAt(toRow, toCol);
    return !targetPiece || targetPiece->getColor() != m_color;
}

std::vector<std::pair<int, int>> King::getPossibleMoves(int row, int col, const Board& board) const {
    std::vector<std::pair<int, int>> moves;
    
    // Toutes les directions possibles pour le roi
    const std::array<std::array<int, 2>, 8> directions = {{{-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1}, {1, 0}, {1, 1}}};

    for (const auto& dir : directions) {
        int newRow = row + dir[0];
        int newCol = col + dir[1];
        
        if (board.isValidPosition(newRow, newCol)) {
            Piece* piece = board.getPieceAt(newRow, newCol);
            if (!piece || piece->getColor() != m_color) {
                moves.push_back({newRow, newCol});
            }
        }
    }
    
    return moves;
}
std::unique_ptr<Piece> King::clone() const {
    auto cloned = std::make_unique<King>(m_color);
    cloned->m_hasMoved = m_hasMoved;
    return cloned;
}

#include "Queen.hpp"
#include "../Board.hpp"
#include <cmath>
#include <vector>
#include <array>

Queen::Queen(Color color) : Piece(color, PieceType::Queen) {}

bool Queen::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const {
    int rowDiff = std::abs(toRow - fromRow);
    int colDiff = std::abs(toCol - fromCol);
    
    // Le déplacement doit être soit diagonal, soit horizontal, soit vertical
    if (rowDiff != colDiff && fromRow != toRow && fromCol != toCol) {
        return false;
    }
    
    // Vérifier que le chemin est dégagé
    if(!isPathClear(fromRow, fromCol, toRow, toCol, board)) {
        return false;
    }

    Piece* targetPiece = board.getPieceAt(toRow, toCol);
    return !targetPiece || targetPiece->getColor() != m_color;
}

std::vector<std::pair<int, int>> Queen::getPossibleMoves(int row, int col, const Board& board) const {
    // Directions: horizontal, vertical, and diagonals (all 8 directions)
    const std::vector<std::array<int, 2>> directions = {
        {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}}
    };
    return getDirectionalMoves(row, col, board, directions);
}

std::unique_ptr<Piece> Queen::clone() const {
    auto cloned = std::make_unique<Queen>(m_color);
    cloned->m_hasMoved = m_hasMoved;
    return cloned;
}

#include "Bishop.hpp"
#include "../Board.hpp"
#include <cmath>


Bishop::Bishop(Color color) : Piece(color, PieceType::Bishop) {}

bool Bishop::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const {
    int rowDiff = std::abs(toRow - fromRow);
    int colDiff = std::abs(toCol - fromCol);
    
    // Le déplacement doit être diagonal
    if (rowDiff != colDiff) {
        return false;
    }
    
    // Vérifier que le chemin est dégagé
    if(!isPathClear(fromRow, fromCol, toRow, toCol, board)) {
        return false;
    }

    Piece* targetPiece = board.getPieceAt(toRow, toCol);
    return !targetPiece || targetPiece->getColor() != m_color;
}

std::vector<std::pair<int, int>> Bishop::getPossibleMoves(int row, int col, const Board& board) const {
    // Diagonal directions
    const std::vector<std::array<int, 2>> directions = {{{-1, -1}, {-1, 1}, {1, -1}, {1, 1}}};
    return getDirectionalMoves(row, col, board, directions);
}

std::unique_ptr<Piece> Bishop::clone() const {
    auto cloned = std::make_unique<Bishop>(m_color);
    cloned->m_hasMoved = m_hasMoved;
    return cloned;
}

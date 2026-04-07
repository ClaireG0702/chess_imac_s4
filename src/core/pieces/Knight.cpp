#include "Knight.hpp"
#include "../Board.hpp"
#include <cmath>
#include <vector>
#include <array>

Knight::Knight(Color color) : Piece(color, PieceType::Knight) {}

bool Knight::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const {
    int rowDiff = std::abs(toRow - fromRow);
    int colDiff = std::abs(toCol - fromCol);
    
    // Le cavalier se déplace en L : 2 cases dans une direction, 1 dans l'autre
    if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
        return false;
    }
    
    // Vérifier la case de destination
    Piece* targetPiece = board.getPieceAt(toRow, toCol);
    return !targetPiece || targetPiece->getColor() != m_color;
}

std::vector<std::pair<int, int>> Knight::getPossibleMoves(int row, int col, const Board& board) const {
    std::vector<std::pair<int, int>> moves;
    
    // Tous les mouvements possibles du cavalier
    const std::array<std::array<int, 2>, 8> knightMoves = {{
        {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
        {1, -2}, {1, 2}, {2, -1}, {2, 1}
    }};
    
    for (const auto& move : knightMoves) {
        int newRow = row + move[0];
        int newCol = col + move[1];
        
        if (board.isValidPosition(newRow, newCol)) {
            Piece* piece = board.getPieceAt(newRow, newCol);
            if (!piece || piece->getColor() != m_color) {
                moves.push_back({newRow, newCol});
            }
        }
    }
    
    return moves;
}
std::unique_ptr<Piece> Knight::clone() const {
    auto cloned = std::make_unique<Knight>(m_color);
    cloned->m_hasMoved = m_hasMoved;
    return cloned;
}

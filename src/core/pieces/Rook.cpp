#include "Rook.hpp"
#include "../Board.hpp"

Rook::Rook(Color color) : Piece(color, PieceType::Rook) {}

bool Rook::isValidMove(int fromRow, int fromCol, int toRow, int toCol, const Board& board) const {
    // Le déplacement doit être soit horizontal soit vertical
    if (fromRow != toRow && fromCol != toCol) {
        return false;
    }
    
    // Vérifier que le chemin est dégagé
    if(!isPathClear(fromRow, fromCol, toRow, toCol, board)) {
        return false;
    }

    Piece* targetPiece = board.getPieceAt(toRow, toCol);
    return !targetPiece || targetPiece->getColor() != m_color;
}

std::vector<std::pair<int, int>> Rook::getPossibleMoves(int row, int col, const Board& board) const {
    std::vector<std::pair<int, int>> moves;
    
    // Directions : haut, bas, gauche, droite
    const std::array<std::array<int, 2>, 4> directions = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1}}};
    
    for (const auto& dir : directions) {
        int newRow = row + dir[0];
        int newCol = col + dir[1];
        
        while (board.isValidPosition(newRow, newCol)) {
            Piece* piece = board.getPieceAt(newRow, newCol);
            
            if (!piece) {
                moves.push_back({newRow, newCol});
            } else {
                if (piece->getColor() != m_color) {
                    moves.push_back({newRow, newCol});
                }
                break;
            }
            
            newRow += dir[0];
            newCol += dir[1];
        }
    }
    
    return moves;
}
std::unique_ptr<Piece> Rook::clone() const {
    auto cloned = std::make_unique<Rook>(m_color);
    cloned->m_hasMoved = m_hasMoved;
    return cloned;
}

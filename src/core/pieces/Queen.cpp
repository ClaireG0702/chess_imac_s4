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
    std::vector<std::pair<int, int>> moves;
    
    // Directions : haut, bas, gauche, droite, et les 4 diagonales
    const std::array<std::array<int, 2>, 8> directions = {{{-1, 0}, {1, 0}, {0, -1}, {0, 1},
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}}};

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
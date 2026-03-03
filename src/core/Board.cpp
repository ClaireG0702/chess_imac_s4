#include "Board.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Rook.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/Queen.hpp"
#include "pieces/King.hpp"

Board::Board() {
    initialize();
}

void Board::clear() {
    for(auto& row : board) {
        for(auto& piece : row) {
            piece.reset();
        }
    }
}

void Board::initialize() {
    clear();

    // Initialiaze white pieces
    for(int col = 0; col < 8; ++col) {
        board[6][col] = std::make_unique<Pawn>(Color::White);
    }
    board[7][0] = std::make_unique<Rook>(Color::White);
    board[7][1] = std::make_unique<Knight>(Color::White);
    board[7][2] = std::make_unique<Bishop>(Color::White);
    board[7][3] = std::make_unique<Queen>(Color::White);
    board[7][4] = std::make_unique<King>(Color::White);
    board[7][5] = std::make_unique<Bishop>(Color::White);
    board[7][6] = std::make_unique<Knight>(Color::White);
    board[7][7] = std::make_unique<Rook>(Color::White);

    // Initialize black pieces
    for(int col = 0; col < 8; ++col) {
        board[1][col] = std::make_unique<Pawn>(Color::Black);
    }
    board[0][0] = std::make_unique<Rook>(Color::Black);
    board[0][1] = std::make_unique<Knight>(Color::Black);
    board[0][2] = std::make_unique<Bishop>(Color::Black);
    board[0][3] = std::make_unique<Queen>(Color::Black);
    board[0][4] = std::make_unique<King>(Color::Black);
    board[0][5] = std::make_unique<Bishop>(Color::Black);
    board[0][6] = std::make_unique<Knight>(Color::Black);
    board[0][7] = std::make_unique<Rook>(Color::Black);
}

Piece *Board::getPieceAt(int row, int col) const {
    if (isValidPosition(row, col)) {
        return board[row][col].get();
    }
    return nullptr;
}

bool Board::setPieceAt(int row, int col, std::unique_ptr<Piece> piece) {
    if (isValidPosition(row, col)) {
        board[row][col] = std::move(piece);
        return true;
    }
    return false;
}

bool Board::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol)) {
        return false;
    }

    Piece* piece = getPieceAt(fromRow, fromCol);
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol)) {
        return false;
    }
    
    board[toRow][toCol] = std::move(board[fromRow][fromCol]);
    return true;
}

bool Board::isCellEmpty(int row, int col) const {
    return getPieceAt(row, col) == nullptr;
}

bool Board::isValidPosition(int row, int col) const {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}
#include "Board.hpp"
#include "Piece.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Rook.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/Queen.hpp"
#include "pieces/King.hpp"

Board::Board() {
}

Board::~Board() {
}

void Board::initialize() {
    pieces.clear();

    // Initialiaze white pieces
    for(int i = 0; i < 8; ++i) {
        pieces.push_back(Pawn(i, 1, Color::White));
    }
    pieces.push_back(Rook(0, 0, Color::White));
    pieces.push_back(Knight(1, 0, Color::White));
    pieces.push_back(Bishop(2, 0, Color::White));
    pieces.push_back(Queen(3, 0, Color::White));
    pieces.push_back(King(4, 0, Color::White));
    pieces.push_back(Bishop(5, 0, Color::White));
    pieces.push_back(Knight(6, 0, Color::White));
    pieces.push_back(Rook(7, 0, Color::White));

    // Initialize black pieces
    for(int i = 0; i < 8; ++i) {
        pieces.push_back(Pawn(i, 6, Color::Black));
    }
    pieces.push_back(Rook(0, 7, Color::Black));
    pieces.push_back(Knight(1, 7, Color::Black));
    pieces.push_back(Bishop(2, 7, Color::Black));
    pieces.push_back(Queen(3, 7, Color::Black));
    pieces.push_back(King(4, 7, Color::Black));
    pieces.push_back(Bishop(5, 7, Color::Black));
    pieces.push_back(Knight(6, 7, Color::Black));
    pieces.push_back(Rook(7, 7, Color::Black));
}

Piece *Board::getPieceAt(int x, int y) const {
    for (const auto& piece : pieces) {
        if (piece.getX() == x && piece.getY() == y) {
            return const_cast<Piece*>(&piece); // Return a pointer to the piece at the given coordinates
        }
    }
    return nullptr; // No piece found at the given coordinates
}


#pragma once
#include "Piece.hpp"
#include <array>
#include <vector>

class Board {
public:
    Board();
    ~Board();

    void initialize();
    Piece *getPieceAt(int x, int y) const;
private:
    std::array<std::array<Piece*, 8>, 8> board; // 8x8 grid of pointers to pieces (nullptr if no piece on the square)
    bool isWhiteToMove;
    std::vector<Piece> pieces;
    
};
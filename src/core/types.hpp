#pragma once
#include <cstdint>

struct Pos {
    int x = 0;
    int y = 0;
};

enum class Color : std::uint8_t { White,
                                  Black };

enum class PieceType : std::uint8_t { Pawn,
                                      Rook,
                                      Knight,
                                      Bishop,
                                      Queen,
                                      King };

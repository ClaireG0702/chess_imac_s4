#pragma once
#include <cstdint>
#include <string>

enum class Color : std::uint8_t { 
    White,
    Black 
};

class Piece {
    public:
        Piece(int x, int y, Color color) : x(x), y(y), color(color) {}
        virtual ~Piece() = default;

        Color getColor() const { return color; }
        int getX() const { return x; }
        int getY() const { return y; }
        std::string toString() const {
            return "Piece at (" + std::to_string(x) + ", " + std::to_string(y) + ") of color " + (color == Color::White ? "White" : "Black");
        }
    private:
        int x, y;
        Color color;
        
};
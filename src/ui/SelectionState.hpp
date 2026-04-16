#pragma once

#include <glm/glm.hpp>

// Enum for cell selection states
enum class CellSelectionState : unsigned char
{
    Inactive,    // Normal state, no interaction
    Hoverable,   // Mouse is hovering over it (but not currently selected)
    Selectable,  // Piece on this cell can be selected (after a piece is selected, shows valid moves)
    Selected     // This cell is currently selected
};

// Structure to represent the state of a single cell
struct CellState
{
    int                  row;
    int                  col;
    CellSelectionState   state;
    glm::vec3            color;  // Will be calculated based on state

    CellState(int r, int c, CellSelectionState s = CellSelectionState::Inactive)
        : row(r), col(c), state(s), color(0.0f, 0.0f, 0.0f)
    {
    }
};

#pragma once
#include "Board.hpp"
#include <cstdint>
#include <utility>

enum class GameStatus : std::uint8_t {
    Playing,
    Check,
    Checkmate,
    Stalemate
};

class GameState {
    public:
        GameState();

        void initialize();
        bool makeMove(int fromRow, int fromCol, int toRow, int toCol);
        bool isValidMove(int fromRow, int fromCol, int toRow, int toCol) const;

        Color getCurrentPlayer() const { return m_currentPlayer; }
        GameStatus getStatus() const { return m_status; }
        const Board& getBoard() const { return m_board; }

        void selectCell(int row, int col);
        void deselectCell();
        bool isCellSelected() const { return m_selectedRow != -1; }
        std::pair<int, int> getSelectedCell() const { return {m_selectedRow, m_selectedCol}; }

    private:
        Board m_board;
        Color m_currentPlayer;
        GameStatus m_status;
        int m_selectedRow;
        int m_selectedCol;

        void switchPlayer();
        void updateGameStatus();
        bool hasValidMoves(Color player) const;
};
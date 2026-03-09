#pragma once
#include "Board.hpp"
#include <cstdint>
#include <utility>

enum class GameStatus : std::uint8_t {
    Playing,
    Check,
    Checkmate,
    Stalemate,
    Victory
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
        Board& getBoard() { return m_board; }

        void selectCell(int row, int col);
        void deselectCell();
        bool isCellSelected() const { return m_selectedRow != -1; }
        std::pair<int, int> getSelectedCell() const { return {m_selectedRow, m_selectedCol}; }
        
        void promotePawn(int row, int col, PieceType newType);
        bool isPromotionPending() const { return m_promotionPending; }
        std::pair<int, int> getPromotionPosition() const { return {m_promotionRow, m_promotionCol}; }

    private:
        Board m_board;
        Color m_currentPlayer;
        GameStatus m_status;
        int m_selectedRow;
        int m_selectedCol;
        bool m_promotionPending;
        int m_promotionRow;
        int m_promotionCol;

        void switchPlayer();
        void updateGameStatus();
        bool hasValidMoves(Color player) const;
        bool isKing(const Piece* piece) const;
};
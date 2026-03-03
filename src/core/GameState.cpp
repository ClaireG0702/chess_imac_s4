#include "GameState.hpp"
#include "core/Piece.hpp"

GameState::GameState() : m_currentPlayer(Color::White), m_status(GameStatus::Playing), m_selectedRow(-1), m_selectedCol(-1) {
    m_board.initialize();
}

void GameState::initialize() {
    m_board.initialize();
    m_currentPlayer = Color::White;
    m_status = GameStatus::Playing;
    
    m_selectedRow = -1;
    m_selectedCol = -1;
}

bool GameState::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    Piece* piece = m_board.getPieceAt(fromRow, fromCol);
    if (piece) {
        piece->setMoved();
    }

    m_board.movePiece(fromRow, fromCol, toRow, toCol);
    switchPlayer();
    updateGameStatus();
    deselectCell();

    return true;
}

bool GameState::isValidMove(int fromRow, int fromCol, int toRow, int toCol) const {
    if (!m_board.isValidPosition(fromRow, fromCol) || !m_board.isValidPosition(toRow, toCol)) {
        return false;
    }

    Piece* piece = m_board.getPieceAt(fromRow, fromCol);
    if (!piece || piece->getColor() != m_currentPlayer) {
        return false;
    }

    return piece->isValidMove(fromRow, fromCol, toRow, toCol, m_board);
}

void GameState::selectCell(int row, int col) {
    if (m_board.isValidPosition(row, col)) {
        m_selectedRow = row;
        m_selectedCol = col;
    }
}

void GameState::deselectCell() {
    m_selectedRow = -1;
    m_selectedCol = -1;
}

void GameState::switchPlayer() {
    m_currentPlayer = (m_currentPlayer == Color::White) ? Color::Black : Color::White;
}

void GameState::updateGameStatus() {
    if (hasValidMoves(m_currentPlayer)) {
        m_status = GameStatus::Playing;
    } else {
        // Vérifier si le roi est en échec
        // (Cette partie nécessite une implémentation pour vérifier l'état du roi)
        bool inCheck = false; // Placeholder, à implémenter

        m_status = inCheck ? GameStatus::Checkmate : GameStatus::Stalemate;
    }
}

bool GameState::hasValidMoves(Color player) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = m_board.getPieceAt(row, col);
            if (piece && piece->getColor() == player) {
                auto moves = piece->getPossibleMoves(row, col, m_board);
                if (!moves.empty()) {
                    return true;
                }
            }
        }
    }
    return false;
}
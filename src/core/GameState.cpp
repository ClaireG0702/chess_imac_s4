#include "GameState.hpp"
#include "core/Piece.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/King.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Rook.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/Knight.hpp"

GameState::GameState() : m_currentPlayer(Color::White), m_status(GameStatus::Playing), m_selectedRow(-1), m_selectedCol(-1), m_promotionPending(false), m_promotionRow(-1), m_promotionCol(-1) {
    m_board.initialize();
}

void GameState::initialize() {
    m_board.initialize();
    m_currentPlayer = Color::White;
    m_status = GameStatus::Playing;
    
    m_selectedRow = -1;
    m_selectedCol = -1;
    m_promotionPending = false;
    m_promotionRow = -1;
    m_promotionCol = -1;
}

bool GameState::makeMove(int fromRow, int fromCol, int toRow, int toCol) {
    if (!isValidMove(fromRow, fromCol, toRow, toCol)) {
        return false;
    }

    Piece* piece = m_board.getPieceAt(fromRow, fromCol);
    if (piece) {
        piece->setMoved();
    }
    
    // Check if capturing the king
    Piece* targetPiece = m_board.getPieceAt(toRow, toCol);
    if (targetPiece && isKing(targetPiece)) {
        m_board.movePiece(fromRow, fromCol, toRow, toCol);
        m_status = GameStatus::Victory;
        deselectCell();
        return true;
    }

    m_board.movePiece(fromRow, fromCol, toRow, toCol);
    
    // Check for pawn promotion
    Piece* movedPiece = m_board.getPieceAt(toRow, toCol);
    if (movedPiece && dynamic_cast<Pawn*>(movedPiece)) {
        int promotionRow = (m_currentPlayer == Color::White) ? 7 : 0;
        if (toRow == promotionRow) {
            m_promotionPending = true;
            m_promotionRow = toRow;
            m_promotionCol = toCol;
            deselectCell();
            return true;
        }
    }
    
    switchPlayer();
    updateGameStatus();
    deselectCell();

    return true;
}

bool GameState::isValidMove(int fromRow, int fromCol, int toRow, int toCol) const {
    // If game is over (Victory), no more moves allowed
    if (m_status == GameStatus::Victory) {
        return false;
    }
    
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
        // Only allow selecting a piece if it belongs to the current player
        Piece* piece = m_board.getPieceAt(row, col);
        if (piece && piece->getColor() == m_currentPlayer) {
            m_selectedRow = row;
            m_selectedCol = col;
        }
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

void GameState::promotePawn(int row, int col, PieceType newType) {
    Color color = m_currentPlayer;
    switch(newType) {
        case PieceType::Queen:
            m_board.setPieceAt(row, col, std::make_unique<Queen>(color));
            break;
        case PieceType::Rook:
            m_board.setPieceAt(row, col, std::make_unique<Rook>(color));
            break;
        case PieceType::Bishop:
            m_board.setPieceAt(row, col, std::make_unique<Bishop>(color));
            break;
        case PieceType::Knight:
            m_board.setPieceAt(row, col, std::make_unique<Knight>(color));
            break;
        default:
            break;
    }
    
    m_promotionPending = false;
    m_promotionRow = -1;
    m_promotionCol = -1;
    
    switchPlayer();
    updateGameStatus();
}

bool GameState::isKing(const Piece* piece) const {
    return piece && dynamic_cast<const King*>(piece);
}
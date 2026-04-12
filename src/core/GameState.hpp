#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "Board.hpp"
#include "GameEvent.hpp"
#include "PieceEvent.hpp"

// Constants for clean code
constexpr int INVALID_POSITION    = -1;
constexpr int MIN_TRANSFORM_PAWNS = 2;
constexpr int MAX_TRANSFORM_PAWNS = 3;
constexpr int WHITE_PROMOTION_ROW = 7;
constexpr int BLACK_PROMOTION_ROW = 0;

enum class GameStatus : std::uint8_t {
    Playing,
    Check,
    Checkmate,
    Stalemate,
    Victory
};

enum class GameMode : std::uint8_t {
    Classic,
    Chaotic
};

struct EventRecord {
    std::string eventName;
    int         turnNumber;
    Color       player;
};

class GameState {
public:
    GameState();

    void     initialize();
    void     setGameMode(GameMode mode) { m_gameMode = mode; }
    GameMode getGameMode() const { return m_gameMode; }
    bool     makeMove(int fromRow, int fromCol, int toRow, int toCol);
    bool     isValidMove(int fromRow, int fromCol, int toRow, int toCol) const;

    Color        getCurrentPlayer() const { return m_currentPlayer; }
    GameStatus   getStatus() const { return m_status; }
    const Board& getBoard() const { return m_board; }
    Board&       getBoard() { return m_board; }

    void                selectCell(int row, int col);
    void                deselectCell();
    bool                isCellSelected() const { return m_selectedRow != INVALID_POSITION; }
    std::pair<int, int> getSelectedCell() const { return {m_selectedRow, m_selectedCol}; }

    void                promotePawn(int row, int col, PieceType newType);
    bool                isPromotionPending() const { return m_promotionPending; }
    std::pair<int, int> getPromotionPosition() const { return {m_promotionRow, m_promotionCol}; }

    // Chaotic mode methods
    void updateChaosEvents();
    bool shouldRevolutionHappen() const;

    // Daltonism getter
    bool isDaltonianMode() const { return m_daltonianMode; }

    // Event history
    const std::vector<EventRecord>& getEventHistory() const { return m_eventHistory; }
    int                             getTurnNumber() const { return m_turnNumber; }

private:
    Board                                    m_board;
    Color                                    m_currentPlayer;
    GameStatus                               m_status;
    GameMode                                 m_gameMode;
    std::unique_ptr<RandomGameEventManager>  m_gameEventManager;
    std::unique_ptr<RandomPieceEventManager> m_pieceEventManager;
    int                                      m_selectedRow;
    int                                      m_selectedCol;
    bool                                     m_promotionPending;
    int                                      m_promotionRow;
    int                                      m_promotionCol;
    std::vector<EventRecord>                 m_eventHistory;
    int                                      m_turnNumber;
    bool                                     m_daltonianMode;
    Color                                    m_originalColors[BOARD_SIZE][BOARD_SIZE]; // Store original colors for restoration
    bool                                     m_colorsSaved;

    void switchPlayer();
    void updateGameStatus();
    bool hasValidMoves(Color player) const;
    bool isKing(const Piece* piece) const;
    void recordEvent(const std::string& eventName);

    // Apply events triggered by moving a piece with characteristics
    void applyCharacteristicEvents(Piece* movedPiece, int toRow, int toCol);

    // Chaotic event helper methods
    void applyGameEvent(std::mt19937& gen);
    void applyPieceEvent(std::mt19937& gen);

    // Chaotic event implementations
    void applyRevolutionEvent();
    void applyDyscalculiaEvent();
    void applyGenderfluidEvent();
    void applyTransEpidemicEvent();
    void applyFurryEvent();
    void applyADHDEvent();
    void applyAutismEvent();
    void applyDaltonismEvent();
};

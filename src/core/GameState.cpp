#include "GameState.hpp"
#include <algorithm>
#include <cmath>
#include "core/Piece.hpp"
#include "pieces/Bishop.hpp"
#include "pieces/King.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Rook.hpp"

GameState::GameState() : m_currentPlayer(Color::White), m_status(GameStatus::Playing), m_gameMode(GameMode::Classic), m_selectedRow(INVALID_POSITION), m_selectedCol(INVALID_POSITION), m_promotionPending(false), m_promotionRow(INVALID_POSITION), m_promotionCol(INVALID_POSITION), m_turnNumber(0), m_daltonianMode(false), m_daltonismTurnsRemaining(0), m_colorsSaved(false), m_rupturAttentionActive(false)
{
    m_board.initialize();
    m_chaosEventManager = nullptr;
}

void GameState::initialize()
{
    m_board.initialize();
    m_currentPlayer = Color::White;
    m_status        = GameStatus::Playing;

    m_selectedRow      = INVALID_POSITION;
    m_selectedCol      = INVALID_POSITION;
    m_promotionPending = false;
    m_promotionRow     = INVALID_POSITION;
    m_promotionCol     = INVALID_POSITION;
    m_turnNumber       = 0;
    m_eventHistory.clear();
    m_daltonianMode           = false;
    m_daltonismTurnsRemaining = 0;
    m_colorsSaved             = false;

    // Initialize unified chaos event manager for chaotic mode
    if (m_gameMode == GameMode::Chaotic)
    {
        m_chaosEventManager = std::make_unique<RandomChaosEventManager>();
    }
    else
    {
        m_chaosEventManager = nullptr;
    }
}

bool GameState::makeMove(int fromRow, int fromCol, int toRow, int toCol)
{
    if (!isValidMove(fromRow, fromCol, toRow, toCol))
    {
        return false;
    }

    Piece* piece = m_board.getPieceAt(fromRow, fromCol);
    if (piece)
    {
        piece->setMoved();
    }

    // RuptureAttention: replace chosen move with random valid move (different from chosen)
    if (m_rupturAttentionActive)
    {
        auto possibleMoves = piece->getPossibleMoves(fromRow, fromCol, m_board);
        
        // Filter out the player's chosen move
        std::vector<std::pair<int, int>> otherMoves;
        for (const auto& [row, col] : possibleMoves)
        {
            if (!(row == toRow && col == toCol))
            {
                otherMoves.push_back({row, col});
            }
        }

        // If there are other valid moves, pick one randomly using Uniform distribution
        if (!otherMoves.empty())
        {
            int selectedIndex = 0;
            if (m_chaosEventManager)
            {
                selectedIndex = m_chaosEventManager->getRandomPieceSelector(0, otherMoves.size() - 1);
                selectedIndex = std::min(selectedIndex, static_cast<int>(otherMoves.size() - 1));
            }
            else
            {
                // Fallback to standard random if manager not available
                std::random_device              rd;
                std::mt19937                    gen(rd());
                std::uniform_int_distribution<> dis(0, otherMoves.size() - 1);
                selectedIndex = dis(gen);
            }
            
            auto [randomRow, randomCol] = otherMoves[selectedIndex];
            toRow = randomRow;
            toCol = randomCol;
        }
        // If no other moves available, use the chosen move
        
        m_rupturAttentionActive = false; // Disable after using
    }

    // Check if capturing the king
    Piece* targetPiece = m_board.getPieceAt(toRow, toCol);
    if (targetPiece && isKing(targetPiece))
    {
        m_board.movePiece(fromRow, fromCol, toRow, toCol);
        m_status = GameStatus::Victory;
        deselectCell();
        return true;
    }

    m_board.movePiece(fromRow, fromCol, toRow, toCol);

    // Check for pawn promotion
    Piece* movedPiece = m_board.getPieceAt(toRow, toCol);
    if (movedPiece && dynamic_cast<Pawn*>(movedPiece))
    {
        int promotionRow = (m_currentPlayer == Color::White) ? WHITE_PROMOTION_ROW : BLACK_PROMOTION_ROW;
        if (toRow == promotionRow)
        {
            m_promotionPending = true;
            m_promotionRow     = toRow;
            m_promotionCol     = toCol;
            deselectCell();
            return true;
        }
    }

    // Apply events triggered by piece characteristics
    applyCharacteristicEvents(movedPiece, toRow, toCol);

    // Increment turn counter
    m_turnNumber++;

    // Update chaos events if in chaotic mode (BEFORE switching player)
    if (m_gameMode == GameMode::Chaotic)
    {
        updateChaosEvents();
    }

    // Handle Daltonism duration: disable after 1 turn (BEFORE switching player)
    if (m_daltonianMode && m_daltonismTurnsRemaining > 0)
    {
        m_daltonismTurnsRemaining--;
        if (m_daltonismTurnsRemaining == 0)
        {
            disableDaltonism();
        }
    }

    switchPlayer();
    updateGameStatus();
    deselectCell();

    return true;
}

bool GameState::isValidMove(int fromRow, int fromCol, int toRow, int toCol) const
{
    // If game is over (Victory), no more moves allowed
    if (m_status == GameStatus::Victory)
    {
        return false;
    }

    if (!m_board.isValidPosition(fromRow, fromCol) || !m_board.isValidPosition(toRow, toCol))
    {
        return false;
    }

    Piece* piece = m_board.getPieceAt(fromRow, fromCol);
    if (!piece)
    {
        return false;
    }

    // In Daltonism mode, allow moving any piece (ignore color check)
    if (!m_daltonianMode && piece->getColor() != m_currentPlayer)
    {
        return false;
    }

    return piece->isValidMove(fromRow, fromCol, toRow, toCol, m_board);
}

void GameState::selectCell(int row, int col)
{
    if (m_board.isValidPosition(row, col))
    {
        Piece* piece = m_board.getPieceAt(row, col);

        // In Daltonism mode, allow selecting any piece (ignore color check)
        if (m_daltonianMode)
        {
            if (piece)
            {
                m_selectedRow = row;
                m_selectedCol = col;
            }
        }
        else
        {
            // Only allow selecting a piece if it belongs to the current player
            if (piece && piece->getColor() == m_currentPlayer)
            {
                m_selectedRow = row;
                m_selectedCol = col;
            }
        }
    }
}

void GameState::deselectCell()
{
    m_selectedRow = INVALID_POSITION;
    m_selectedCol = INVALID_POSITION;
}

void GameState::switchPlayer()
{
    m_currentPlayer = (m_currentPlayer == Color::White) ? Color::Black : Color::White;
}

void GameState::updateGameStatus()
{
    if (hasValidMoves(m_currentPlayer))
    {
        m_status = GameStatus::Playing;
    }
    else
    {
        // Vérifier si le roi est en échec
        // (Cette partie nécessite une implémentation pour vérifier l'état du roi)
        bool inCheck = false; // Placeholder, à implémenter

        m_status = inCheck ? GameStatus::Checkmate : GameStatus::Stalemate;
    }
}

bool GameState::hasValidMoves(Color player) const
{
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            if (piece && piece->getColor() == player)
            {
                auto moves = piece->getPossibleMoves(row, col, m_board);
                if (!moves.empty())
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void GameState::promotePawn(int row, int col, PieceType newType)
{
    Color color = m_currentPlayer;
    switch (newType)
    {
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
    m_promotionRow     = INVALID_POSITION;
    m_promotionCol     = INVALID_POSITION;

    switchPlayer();
    updateGameStatus();
}

bool GameState::isKing(const Piece* piece) const
{
    return piece && dynamic_cast<const King*>(piece);
}

void GameState::updateChaosEvents()
{
    if (m_gameMode != GameMode::Chaotic || !m_chaosEventManager)
    {
        return;
    }

    m_chaosEventManager->updateTurn();

    // 45% chance qu'UN événement GLOBAL se déclenche par tour
    std::random_device               rd;
    std::mt19937                     gen(rd());
    std::uniform_real_distribution<> eventProbability(0.0, 1.0);

    const double EVENT_TRIGGER_CHANCE = 0.45; // 45% chance per turn

    if (eventProbability(gen) < EVENT_TRIGGER_CHANCE)
    {
        // Apply game events only (global board-affecting events)
        applyGameEvent(gen);
    }
}

void GameState::applyGameEvent(std::mt19937& gen)
{
    if (!m_chaosEventManager)
        return;

    // Select a random game event and trigger based on its distribution
    std::uniform_int_distribution<> eventDistribution(0, 3);
    int                             selectedEvent = eventDistribution(gen);

    switch (selectedEvent)
    {
    case 0:
        // Revolution: Uses Binomial distribution
        if (m_chaosEventManager->shouldRevolutionHappen())
        {
            applyRevolutionEvent();
        }
        break;
    case 1:
        // TransEpidemic: Uses Poisson distribution
        // If Poisson returns > 0, event can trigger
        if (m_chaosEventManager->getEpidemicTransformCount() > 0)
        {
            applyTransEpidemicEvent();
        }
        break;
    case 2:
        // CavalierSauvage: Uses UniformDiscrete distribution for piece selection
        // Exponential distribution determines trigger timing
        if (m_chaosEventManager->getMutationChangeTime() > 0.5)
        {
            applyCavalierSauvageEvent();
        }
        break;
    case 3:
        // Daltonism: Uses ChiSquared distribution
        // If confusion level is significant, trigger
        if (m_chaosEventManager->getDaltonismConfusion() > 0.5)
        {
            applyDaltonismEvent();
        }
        break;
    default:
        break;
    }
}

void GameState::applyPieceEvent(std::mt19937& gen)
{
    if (!m_chaosEventManager)
        return;

    // Select a random piece event and trigger based on its distribution
    std::uniform_int_distribution<> eventDistribution(0, 3);
    int                             selectedEvent = eventDistribution(gen);

    switch (selectedEvent)
    {
    case 0:
        // Dyscalculia: Uses Normal distribution
        // If error magnitude is significant, trigger
        if (std::abs(m_chaosEventManager->getDyscalculiaError()) > 0.5)
        {
            applyDyscalculiaEvent();
        }
        break;
    case 1:
        // Mutation: Uses Exponential distribution
        // If time value exceeds threshold, trigger
        if (m_chaosEventManager->getMutationChangeTime() > 0.5)
        {
            applyMutationEvent();
        }
        break;
    case 2:
        // RuptureAttention: Uses Cauchy distribution (heavy tails)
        // If chaotic value is high, trigger
        if (std::abs(m_chaosEventManager->getRuptureAttentionRandomBehavior()) > 1.0)
        {
            applyRuptureAttentionEvent();
        }
        break;
    case 3:
        // Fuite: Uses Exponential distribution
        // Rare triggering based on exponential timing
        if (m_chaosEventManager->getMutationChangeTime() > 1.5)
        {
            applyFuitevent();
        }
        break;
    default:
        break;
    }
}

bool GameState::shouldRevolutionHappen() const
{
    if (!m_chaosEventManager)
    {
        return false;
    }
    return m_chaosEventManager->shouldRevolutionHappen();
}

void GameState::applyRevolutionEvent()
{
    if (!m_chaosEventManager)
        return;
    recordEvent("Révolution");

    // Find all pawns on the board (any color)
    std::vector<std::pair<int, int>> allPawns;

    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            // Check if it's a pawn (any color)
            if (piece && dynamic_cast<Pawn*>(piece))
            {
                allPawns.push_back({row, col});
            }
        }
    }

    if (allPawns.empty())
        return;

    // Use Binomial distribution to determine number of pawns affected (2-3)
    int countToTransform = m_chaosEventManager->getRevolutionAffectedCount();
    
    // Clamp to 2-3 pawns
    countToTransform = std::max(2, std::min(3, countToTransform));
    
    // Make sure we don't try to transform more pawns than available
    countToTransform = std::min(countToTransform, static_cast<int>(allPawns.size()));

    // Hypergeometric: selection without replacement (shuffle ensures no duplicates)
    std::random_device rd;
    std::mt19937       gen(rd());
    std::shuffle(allPawns.begin(), allPawns.end(), gen);

    // Invert the color of selected pawns
    for (int i = 0; i < countToTransform; ++i)
    {
        int row = allPawns[i].first;
        int col = allPawns[i].second;

        Piece* piece = m_board.getPieceAt(row, col);
        if (piece)
        {
            // Invert the color: White becomes Black, Black becomes White
            Color invertedColor = (piece->getColor() == Color::White) ? Color::Black : Color::White;
            piece->setColor(invertedColor);
        }
    }
}
// Dyscalculia: Normal - pieces move but miscalculate their distance
void GameState::applyDyscalculiaEvent()
{
    if (!m_chaosEventManager)
        return;
    recordEvent("Dyscalculia");

    // Get all pieces on the board
    std::vector<std::pair<int, int>> pieces;
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            if (m_board.getPieceAt(row, col) != nullptr && !isKing(m_board.getPieceAt(row, col)))
            {
                pieces.push_back({row, col});
            }
        }
    }

    if (!pieces.empty())
    {
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> pieceDis(0, pieces.size() - 1);

        auto [row, col] = pieces[pieceDis(gen)];
        Piece* piece    = m_board.getPieceAt(row, col);

        if (piece)
        {
            // Get all possible moves for this piece
            auto possibleMoves = piece->getPossibleMoves(row, col, m_board);

            if (!possibleMoves.empty())
            {
                // Select a random valid move
                std::uniform_int_distribution<> moveDis(0, possibleMoves.size() - 1);
                auto [moveRow, moveCol] = possibleMoves[moveDis(gen)];

                // Calculate the direction and distance of this move
                int dirRow   = (moveRow == row) ? 0 : (moveRow > row) ? 1
                                                                      : -1;
                int dirCol   = (moveCol == col) ? 0 : (moveCol > col) ? 1
                                                                      : -1;
                int distance = std::max(std::abs(moveRow - row), std::abs(moveCol - col));

                // Use Cauchy distribution for movement error (heavy tail = more chaotic errors)
                double cauchyError = m_chaosEventManager->getRuptureAttentionRandomBehavior();
                int    errorOffset = static_cast<int>(std::abs(cauchyError)); // Take absolute value for distance offset

                int incorrectDistance = distance + errorOffset;

                if (incorrectDistance > 0)
                {
                    int incorrectRow = row + (dirRow * incorrectDistance);
                    int incorrectCol = col + (dirCol * incorrectDistance);

                    // Check if the incorrect move destination is valid and empty
                    if (m_board.isValidPosition(incorrectRow, incorrectCol) && m_board.getPieceAt(incorrectRow, incorrectCol) == nullptr)
                    {
                        m_board.movePiece(row, col, incorrectRow, incorrectCol);
                    }
                }
            }
        }
    }
}

// Mutation: Exponential - pieces change type over time
void GameState::applyMutationEvent()
{
    if (!m_chaosEventManager)
        return;
    recordEvent("Mutation");

    // Randomly select a piece and change its type
    std::vector<std::pair<int, int>> transformablePieces;

    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            if (piece && !isKing(piece))
            {
                transformablePieces.push_back({row, col});
            }
        }
    }

    if (!transformablePieces.empty())
    {
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> pieceDis(0, transformablePieces.size() - 1);

        auto [row, col] = transformablePieces[pieceDis(gen)];
        Piece* piece    = m_board.getPieceAt(row, col);
        if (piece)
        {
            Color color = piece->getColor();
            
            // Use Chi-squared distribution to determine the new piece type
            double chiSquaredValue = m_chaosEventManager->getDaltonismConfusion();
            int typeIndex = static_cast<int>(chiSquaredValue) % 5;
            
            PieceType types[] = {PieceType::Pawn, PieceType::Knight, PieceType::Bishop, PieceType::Rook, PieceType::Queen};
            PieceType newType = types[typeIndex];

            std::unique_ptr<Piece> newPiece;
            switch (newType)
            {
            case PieceType::Pawn: newPiece = std::make_unique<Pawn>(color); break;
            case PieceType::Knight: newPiece = std::make_unique<Knight>(color); break;
            case PieceType::Bishop: newPiece = std::make_unique<Bishop>(color); break;
            case PieceType::Rook: newPiece = std::make_unique<Rook>(color); break;
            case PieceType::Queen: newPiece = std::make_unique<Queen>(color); break;
            default: newPiece = std::make_unique<Pawn>(color); break;
            }
            m_board.setPieceAt(row, col, std::move(newPiece));
        }
    }
}

// Trans Epidemic: Poisson - transforms Pawns into Queens and Queens into Pawns
void GameState::applyTransEpidemicEvent()
{
    if (!m_chaosEventManager)
        return;
    recordEvent("SwapReinePion");

    // Find all Pawns and Queens on the board
    std::vector<std::pair<int, int>> pawnOrQueen;
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            if (piece)
            {
                Pawn*  pawn  = dynamic_cast<Pawn*>(piece);
                Queen* queen = dynamic_cast<Queen*>(piece);
                if (pawn || queen)
                {
                    pawnOrQueen.push_back({row, col});
                }
            }
        }
    }

    if (!pawnOrQueen.empty())
    {
        // Use UniformDiscrete distribution to select a Pawn or Queen
        int selectedIndex = m_chaosEventManager->getRandomPieceSelector(0, pawnOrQueen.size() - 1);
        
        // Make sure index is valid
        selectedIndex = std::min(selectedIndex, static_cast<int>(pawnOrQueen.size() - 1));

        auto [row, col] = pawnOrQueen[selectedIndex];
        Piece* piece    = m_board.getPieceAt(row, col);

        if (piece)
        {
            Color color = piece->getColor();
            Pawn* pawn  = dynamic_cast<Pawn*>(piece);

            std::unique_ptr<Piece> newPiece;
            if (pawn)
            {
                // Transform Pawn into Queen
                newPiece = std::make_unique<Queen>(color);
            }
            else
            {
                // Transform Queen into Pawn
                newPiece = std::make_unique<Pawn>(color);
            }

            m_board.setPieceAt(row, col, std::move(newPiece));
        }
    }
}

// CavalierSauvage: UniformDiscrete - transforms a random piece into a Knight
void GameState::applyCavalierSauvageEvent()
{
    if (!m_chaosEventManager)
        return;
    recordEvent("CavalierSauvage");

    std::vector<std::pair<int, int>> pieces;
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            if (m_board.getPieceAt(row, col) != nullptr && !isKing(m_board.getPieceAt(row, col)))
            {
                pieces.push_back({row, col});
            }
        }
    }

    if (!pieces.empty())
    {
        // Use UniformDiscrete distribution to select a random piece
        int selectedIndex = m_chaosEventManager->getRandomPieceSelector(0, pieces.size() - 1);
        
        // Make sure index is valid
        selectedIndex = std::min(selectedIndex, static_cast<int>(pieces.size() - 1));

        auto [row, col] = pieces[selectedIndex];
        Piece* piece    = m_board.getPieceAt(row, col);

        if (piece)
        {
            Color color = piece->getColor();
            // Transform the piece into a Knight
            std::unique_ptr<Piece> newPiece = std::make_unique<Knight>(color);
            m_board.setPieceAt(row, col, std::move(newPiece));
        }
    }
}

// RuptureAttention: Cauchy - player's next move will be random instead of chosen
void GameState::applyRuptureAttentionEvent()
{
    if (!m_chaosEventManager)
        return;
    recordEvent("RuptureAttention");

    // Activate the flag: next player move will be random
    m_rupturAttentionActive = true;
}

// Fuite: Hypergeometric - forces current player's piece to move to isolated square, ends turn
void GameState::applyFuitevent()
{
    if (!m_chaosEventManager)
        return;
    recordEvent("Fuite");

    // Find all pieces of CURRENT PLAYER that can move to isolated squares
    std::vector<std::pair<int, int>> validPieces;

    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            // Filter: current player's piece, not a king, and has valid moves
            if (piece && piece->getColor() == m_currentPlayer && !isKing(piece))
            {
                // Get possible moves for this piece
                auto possibleMoves = piece->getPossibleMoves(row, col, m_board);

                // Check if any move leads to an isolated square
                for (const auto& [moveRow, moveCol] : possibleMoves)
                {
                    // Check if this destination is isolated (no neighbors)
                    bool isIsolated = true;
                    for (int dr = -1; dr <= 1; ++dr)
                    {
                        for (int dc = -1; dc <= 1; ++dc)
                        {
                            if (dr == 0 && dc == 0)
                                continue; // Skip the center square itself
                            int checkRow = moveRow + dr;
                            int checkCol = moveCol + dc;

                            // Check if position is valid and has a piece
                            if (m_board.isValidPosition(checkRow, checkCol))
                            {
                                if (m_board.getPieceAt(checkRow, checkCol) != nullptr)
                                {
                                    isIsolated = false;
                                    break;
                                }
                            }
                        }
                        if (!isIsolated)
                            break;
                    }

                    // If we found an isolated destination, add this piece
                    if (isIsolated)
                    {
                        validPieces.push_back({row, col});
                        break; // Found at least one valid isolated move for this piece
                    }
                }
            }
        }
    }

    // If there are pieces of current player that can move to isolated squares
    if (!validPieces.empty())
    {
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> pieceDis(0, validPieces.size() - 1);

        auto [row, col] = validPieces[pieceDis(gen)];
        Piece* piece    = m_board.getPieceAt(row, col);

        if (piece)
        {
            // Get all possible moves that lead to isolated squares
            auto                             possibleMoves = piece->getPossibleMoves(row, col, m_board);
            std::vector<std::pair<int, int>> isolatedMoves;

            for (const auto& [moveRow, moveCol] : possibleMoves)
            {
                bool isIsolated = true;
                for (int dr = -1; dr <= 1; ++dr)
                {
                    for (int dc = -1; dc <= 1; ++dc)
                    {
                        if (dr == 0 && dc == 0)
                            continue;
                        int checkRow = moveRow + dr;
                        int checkCol = moveCol + dc;

                        if (m_board.isValidPosition(checkRow, checkCol))
                        {
                            if (m_board.getPieceAt(checkRow, checkCol) != nullptr)
                            {
                                isIsolated = false;
                                break;
                            }
                        }
                    }
                    if (!isIsolated)
                        break;
                }

                if (isIsolated)
                {
                    isolatedMoves.push_back({moveRow, moveCol});
                }
            }

            // Move to a random isolated square
            if (!isolatedMoves.empty())
            {
                std::uniform_int_distribution<> moveDis(0, isolatedMoves.size() - 1);
                auto [toRow, toCol] = isolatedMoves[moveDis(gen)];
                m_board.movePiece(row, col, toRow, toCol);
                
                // Turn ends: switch to next player
                switchPlayer();
            }
        }
    }
}

// Daltonism: Makes all pieces appear gray and allows playing any piece
void GameState::applyDaltonismEvent()
{
    if (!m_chaosEventManager)
        return;
    recordEvent("Daltonisme");

    // Save original colors if not already saved
    if (!m_colorsSaved)
    {
        for (int row = 0; row < BOARD_SIZE; ++row)
        {
            for (int col = 0; col < BOARD_SIZE; ++col)
            {
                Piece* piece = m_board.getPieceAt(row, col);
                if (piece)
                {
                    m_originalColors[row][col] = piece->getColor();
                }
                else
                {
                    m_originalColors[row][col] = Color::White; // Default value for empty squares
                }
            }
        }
        m_colorsSaved = true;
    }

    // Change all pieces to gray color
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            if (piece)
            {
                piece->setColor(Color::Gray);
            }
        }
    }

    // Use Normal distribution to determine chaos intensity
    // Higher absolute value = stronger effect duration
    double chaosIntensity = std::abs(m_chaosEventManager->getDyscalculiaError());
    
    // Map intensity to turn duration: base 1 turn, can extend based on intensity
    // Clamp to 1 turn (intensity mostly controls the visual chaos)
    int turnDuration = 2; // 2 because we decrement at end of move, so 1 turn for player
    
    m_daltonianMode           = true;
    m_daltonismTurnsRemaining = turnDuration;
}

void GameState::applyCharacteristicEvents(Piece* movedPiece, int toRow, int toCol)
{
    if (!movedPiece || !m_chaosEventManager)
    {
        return;
    }

    // Generate random value for piece events (45% chance each)
    std::random_device               rd;
    std::mt19937                     gen(rd());
    std::uniform_real_distribution<> eventTrigger(0.0, 1.0);
    const double PIECE_EVENT_CHANCE = 0.45; // 45% chance per piece characteristic

    m_chaosEventManager->updateTurn();

    // Check Dyscalculia characteristic - trigger randomly if present
    if (movedPiece->hasCharacteristic(PieceCharacteristic::Dyscalculia))
    {
        if (eventTrigger(gen) < PIECE_EVENT_CHANCE)
        {
            applyDyscalculiaEvent();
        }
    }

    // Check Mutation characteristic - trigger randomly if present
    if (movedPiece->hasCharacteristic(PieceCharacteristic::Mutation))
    {
        if (eventTrigger(gen) < PIECE_EVENT_CHANCE)
        {
            applyMutationEvent();
        }
    }

    // Check RuptureAttention characteristic - trigger randomly if present
    if (movedPiece->hasCharacteristic(PieceCharacteristic::RuptureAttention))
    {
        if (eventTrigger(gen) < PIECE_EVENT_CHANCE)
        {
            applyRuptureAttentionEvent();
        }
    }

    // Check Fuite characteristic - trigger randomly if present
    if (movedPiece->hasCharacteristic(PieceCharacteristic::Fuite))
    {
        if (eventTrigger(gen) < PIECE_EVENT_CHANCE)
        {
            applyFuitevent();
        }
    }
}

void GameState::recordEvent(const std::string& eventName)
{
    m_eventHistory.push_back({eventName, m_turnNumber});
}

void GameState::disableDaltonism()
{
    if (!m_daltonianMode || !m_colorsSaved)
        return;

    // Restore original colors for all pieces
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            if (piece)
            {
                piece->setColor(m_originalColors[row][col]);
            }
        }
    }

    // Disable Daltonism mode
    m_daltonianMode           = false;
    m_daltonismTurnsRemaining = 0;
    m_colorsSaved             = false;
}

ClickResult GameState::handleCellClick(int row, int col)
{
    // Validate position
    if (!m_board.isValidPosition(row, col))
    {
        return ClickResult(ClickAction::None, true, "Invalid position");
    }

    Piece* clickedPiece = m_board.getPieceAt(row, col);

    // Case 1: No piece currently selected
    if (!isCellSelected())
    {
        // Try to select the clicked cell
        if (!clickedPiece)
        {
            return ClickResult(ClickAction::None, true, "Empty cell");
        }

        // Check if piece belongs to current player (or any piece in Daltonism mode)
        if (!m_daltonianMode && clickedPiece->getColor() != m_currentPlayer)
        {
            return ClickResult(ClickAction::None, true, "Wrong color - not your piece");
        }

        // Select the piece
        selectCell(row, col);
        return ClickResult(ClickAction::Selected);
    }

    // Case 2: A piece is currently selected
    auto [selectedRow, selectedCol] = getSelectedCell();

    // Case 2a: Clicked on the same cell → deselect
    if (row == selectedRow && col == selectedCol)
    {
        deselectCell();
        return ClickResult(ClickAction::Deselected);
    }

    // Case 2b: Try to move to the clicked cell
    if (isValidMove(selectedRow, selectedCol, row, col))
    {
        bool moveMade = makeMove(selectedRow, selectedCol, row, col);
        if (moveMade)
        {
            return ClickResult(ClickAction::MoveExecuted);
        }
        else
        {
            return ClickResult(ClickAction::None, true, "Move execution failed");
        }
    }

    // Case 2c: Invalid move - deselect and consider new selection
    deselectCell();

    if (!clickedPiece)
    {
        return ClickResult(ClickAction::None, true, "Invalid move - clicked empty cell");
    }

    // Try to select the new piece if it belongs to current player
    if (m_daltonianMode || clickedPiece->getColor() == m_currentPlayer)
    {
        selectCell(row, col);
        return ClickResult(ClickAction::Selected);
    }
    else
    {
        return ClickResult(ClickAction::None, true, "Invalid move - wrong color");
    }
}
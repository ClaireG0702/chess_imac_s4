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

GameState::GameState() : m_currentPlayer(Color::White), m_status(GameStatus::Playing), m_gameMode(GameMode::Classic), m_selectedRow(INVALID_POSITION), m_selectedCol(INVALID_POSITION), m_promotionPending(false), m_promotionRow(INVALID_POSITION), m_promotionCol(INVALID_POSITION), m_turnNumber(0), m_daltonianMode(false), m_colorsSaved(false)
{
    m_board.initialize();
    m_gameEventManager  = nullptr;
    m_pieceEventManager = nullptr;
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
    m_daltonianMode = false;
    m_colorsSaved   = false;

    // Initialize event managers for chaotic mode
    if (m_gameMode == GameMode::Chaotic)
    {
        m_gameEventManager  = std::make_unique<RandomGameEventManager>();
        m_pieceEventManager = std::make_unique<RandomPieceEventManager>();
    }
    else
    {
        m_gameEventManager  = nullptr;
        m_pieceEventManager = nullptr;
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

    switchPlayer();
    updateGameStatus();
    deselectCell();

    // Increment turn counter
    m_turnNumber++;

    // Update chaos events if in chaotic mode
    if (m_gameMode == GameMode::Chaotic)
    {
        updateChaosEvents();
    }

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
    if (m_gameMode != GameMode::Chaotic || !m_gameEventManager || !m_pieceEventManager)
    {
        return;
    }

    m_gameEventManager->updateTurn();
    m_pieceEventManager->updateTurn();

    // Calcul global: 45% de chance qu'UN événement se déclenche par tour
    std::random_device               rd;
    std::mt19937                     gen(rd());
    std::uniform_real_distribution<> eventProbability(0.0, 1.0);

    const double EVENT_TRIGGER_CHANCE = 0.45; // 45% chance per turn

    if (eventProbability(gen) < EVENT_TRIGGER_CHANCE)
    {
        // Choisir entre un événement du jeu (50%) ou un événement de pion (50%)
        std::uniform_real_distribution<> categoryProbability(0.0, 1.0);
        bool                             isGameEvent = categoryProbability(gen) < 0.5;

        if (isGameEvent)
        {
            applyGameEvent(gen);
        }
        else
        {
            applyPieceEvent(gen);
        }
    }
}

void GameState::applyGameEvent(std::mt19937& gen)
{
    std::uniform_int_distribution<> eventDistribution(0, 3);
    int                             selectedEvent = eventDistribution(gen);

    switch (selectedEvent)
    {
    case 0:
        if (shouldRevolutionHappen())
        {
            applyRevolutionEvent();
        }
        break;
    case 1:
        applyTransEpidemicEvent();
        break;
    case 2:
        applyFurryEvent();
        break;
    case 3:
        applyDaltonismEvent();
        break;
    default:
        break;
    }
}

void GameState::applyPieceEvent(std::mt19937& gen)
{
    std::uniform_int_distribution<> eventDistribution(0, 3);
    int                             selectedEvent = eventDistribution(gen);

    switch (selectedEvent)
    {
    case 0:
        applyDyscalculiaEvent();
        break;
    case 1:
        applyGenderfluidEvent();
        break;
    case 2:
        applyADHDEvent();
        break;
    case 3:
        applyAutismEvent();
        break;
    default:
        break;
    }
}

bool GameState::shouldRevolutionHappen() const
{
    if (!m_gameEventManager)
    {
        return false;
    }
    return m_gameEventManager->shouldRevolutionHappen();
}

void GameState::applyRevolutionEvent()
{
    if (!m_gameEventManager)
        return;
    recordEvent("Révolution");

    // Find all pawns of the current player
    std::vector<std::pair<int, int>> playerPawns;

    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            // Check if it's a pawn of the current player
            if (piece && piece->getColor() == m_currentPlayer && dynamic_cast<Pawn*>(piece))
            {
                playerPawns.push_back({row, col});
            }
        }
    }

    if (playerPawns.empty())
        return;

    // Randomly shuffle and select 2-3 pawns to swap color
    std::random_device rd;
    std::mt19937       gen(rd());
    std::shuffle(playerPawns.begin(), playerPawns.end(), gen);

    // Select 2-3 pawns to transform
    int                             numToTransform = std::min(3, static_cast<int>(playerPawns.size()));
    std::uniform_int_distribution<> dis(2, numToTransform);
    int                             countToTransform = dis(gen);

    // Invert the color of selected pawns
    for (int i = 0; i < countToTransform && i < static_cast<int>(playerPawns.size()); ++i)
    {
        int row = playerPawns[i].first;
        int col = playerPawns[i].second;

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
    if (!m_pieceEventManager)
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

                // Apply incorrect distance: distance +/- 1
                std::uniform_int_distribution<> distanceDis(0, 1);
                int                             incorrectDistance = distance + (distanceDis(gen) == 0 ? 1 : -1);

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

// Genderfluid: Exponential - pieces change type over time
void GameState::applyGenderfluidEvent()
{
    if (!m_pieceEventManager)
        return;
    recordEvent("Genderfluid");

    double changeTime = m_pieceEventManager->getGenderfluidChangeTime();

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
        std::uniform_int_distribution<> dis(0, transformablePieces.size() - 1);

        auto [row, col] = transformablePieces[dis(gen)];
        Piece* piece    = m_board.getPieceAt(row, col);
        if (piece)
        {
            Color     color   = piece->getColor();
            PieceType types[] = {PieceType::Pawn, PieceType::Knight, PieceType::Bishop, PieceType::Rook, PieceType::Queen};
            PieceType newType = types[dis(gen) % 5];

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
    if (!m_gameEventManager)
        return;
    recordEvent("Épidémie");

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
        std::random_device              rd;
        std::mt19937                    gen(rd());
        std::uniform_int_distribution<> dis(0, pawnOrQueen.size() - 1);

        // Select one random Pawn or Queen
        auto [row, col] = pawnOrQueen[dis(gen)];
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

// Furry: UniformDiscrete - transforms a random piece into a Knight
void GameState::applyFurryEvent()
{
    if (!m_gameEventManager)
        return;
    recordEvent("Furry");

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
        std::uniform_int_distribution<> dis(0, pieces.size() - 1);

        // Select one random piece
        auto [row, col] = pieces[dis(gen)];
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

// ADHD: Cauchy - random chaotic behavior - pieces move to random valid destinations
void GameState::applyADHDEvent()
{
    if (!m_pieceEventManager)
        return;
    recordEvent("ADHD");

    double chaotic = m_pieceEventManager->getADHDRandomBehavior();

    // Select random pieces and move them to random valid destinations
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
        std::random_device rd;
        std::mt19937       gen(rd());

        // Select 1 random piece
        std::uniform_int_distribution<> pieceDis(0, pieces.size() - 1);
        auto [row, col] = pieces[pieceDis(gen)];

        Piece* piece = m_board.getPieceAt(row, col);
        if (piece)
        {
            // Get all possible moves for this piece
            auto possibleMoves = piece->getPossibleMoves(row, col, m_board);

            if (possibleMoves.size() >= 2)
            {
                // Remove the original position from possible moves if it exists
                possibleMoves.erase(
                    std::remove_if(possibleMoves.begin(), possibleMoves.end(), [row, col](const std::pair<int, int>& move) {
                        return move.first == row && move.second == col;
                    }),
                    possibleMoves.end()
                );

                // Select a random move from the remaining possible moves
                if (!possibleMoves.empty())
                {
                    std::uniform_int_distribution<> moveDis(0, possibleMoves.size() - 1);
                    auto [toRow, toCol] = possibleMoves[moveDis(gen)];

                    // Move the piece to the random position
                    m_board.movePiece(row, col, toRow, toCol);
                }
            }
        }
    }
}

// Autism: Hypergeometric - forces pieces to move only to isolated squares
void GameState::applyAutismEvent()
{
    if (!m_pieceEventManager)
        return;
    recordEvent("Autism");

    // Find all pieces that can move to isolated squares
    std::vector<std::pair<int, int>> validPieces;

    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = m_board.getPieceAt(row, col);
            if (piece && !isKing(piece))
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

    // If there are pieces that can move to isolated squares
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
            }
        }
    }
}

// Daltonism: Makes all pieces appear gray and allows playing any piece
void GameState::applyDaltonismEvent()
{
    if (!m_gameEventManager)
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

    // Enable Daltonism mode
    m_daltonianMode = true;
}

void GameState::applyCharacteristicEvents(Piece* movedPiece, int toRow, int toCol)
{
    if (!movedPiece || !m_pieceEventManager)
    {
        return;
    }

    std::uint8_t characteristics = movedPiece->getCharacteristics();

    // Check Dyscalculia characteristic
    if (movedPiece->hasCharacteristic(PieceCharacteristic::Dyscalculia))
    {
        double error = m_pieceEventManager->getDyscalculiaError();
        if (std::abs(error) > 0.5)
        { // If error magnitude is significant
            applyDyscalculiaEvent();
        }
    }

    // Check Genderfluid characteristic
    if (movedPiece->hasCharacteristic(PieceCharacteristic::Genderfluid))
    {
        double changeTime = m_pieceEventManager->getGenderfluidChangeTime();
        if (changeTime > 0.5)
        { // If time value exceeds threshold
            applyGenderfluidEvent();
        }
    }

    // Check ADHD characteristic
    if (movedPiece->hasCharacteristic(PieceCharacteristic::ADHD))
    {
        double chaotic = m_pieceEventManager->getADHDRandomBehavior();
        if (std::abs(chaotic) > 1.0)
        { // If chaotic value is high (Cauchy heavy tails)
            applyADHDEvent();
        }
    }

    // Check Autism characteristic
    if (movedPiece->hasCharacteristic(PieceCharacteristic::Autism))
    {
        int isolatedCount = m_pieceEventManager->getAutismIsolatedCount();
        if (isolatedCount > 10)
        { // If isolation count exceeds threshold
            applyAutismEvent();
        }
    }
}

void GameState::recordEvent(const std::string& eventName)
{
    m_eventHistory.push_back({eventName, m_turnNumber, m_currentPlayer});
}
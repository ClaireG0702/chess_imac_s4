#include "Board.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include "pieces/Bishop.hpp"
#include "pieces/King.hpp"
#include "pieces/Knight.hpp"
#include "pieces/Pawn.hpp"
#include "pieces/Queen.hpp"
#include "pieces/Rook.hpp"

// Probability constants for piece characteristics
constexpr double Fuite_PROBABILITY            = 0.0105; // 1.05%
constexpr double RuptureAttention_PROBABILITY = 0.05;   // 5%
constexpr double DYSCALCULIA_PROBABILITY      = 0.0295; // 2.95%
constexpr double Mutation_PROBABILITY         = 0.0145; // 1.45%

Board::Board()
{
    initialize();
}

void Board::clear()
{
    for (auto& row : board)
    {
        for (auto& piece : row)
        {
            piece.reset();
        }
    }
}

void Board::initialize()
{
    clear();

    // Initialize white pieces (at bottom, indices 0-1)
    for (int col = 0; col < BOARD_SIZE; ++col)
    {
        board[1][col] = std::make_unique<Pawn>(Color::White);
    }
    board[0][0] = std::make_unique<Rook>(Color::White);
    board[0][1] = std::make_unique<Knight>(Color::White);
    board[0][2] = std::make_unique<Bishop>(Color::White);
    board[0][3] = std::make_unique<Queen>(Color::White);
    board[0][4] = std::make_unique<King>(Color::White);
    board[0][5] = std::make_unique<Bishop>(Color::White);
    board[0][6] = std::make_unique<Knight>(Color::White);
    board[0][7] = std::make_unique<Rook>(Color::White);

    // Initialize black pieces (at top, indices 6-7)
    for (int col = 0; col < BOARD_SIZE; ++col)
    {
        board[6][col] = std::make_unique<Pawn>(Color::Black);
    }
    board[7][0] = std::make_unique<Rook>(Color::Black);
    board[7][1] = std::make_unique<Knight>(Color::Black);
    board[7][2] = std::make_unique<Bishop>(Color::Black);
    board[7][3] = std::make_unique<Queen>(Color::Black);
    board[7][4] = std::make_unique<King>(Color::Black);
    board[7][5] = std::make_unique<Bishop>(Color::Black);
    board[7][6] = std::make_unique<Knight>(Color::Black);
    board[7][7] = std::make_unique<Rook>(Color::Black);

    // Assign characteristics to pieces based on percentages
    assignCharacteristicsToBoard();
}

void Board::assignCharacteristicsToBoard()
{
    std::vector<std::pair<int, int>> piecesWithoutKings = getAllPiecesExceptKings();

    int totalPieces = piecesWithoutKings.size();

    // Calculate number of pieces to assign for each characteristic
    int FuiteCount            = std::round(totalPieces * Fuite_PROBABILITY);
    int RuptureAttentionCount = std::round(totalPieces * RuptureAttention_PROBABILITY);
    int dyscalculiaCount      = std::round(totalPieces * DYSCALCULIA_PROBABILITY);
    int MutationCount         = std::round(totalPieces * Mutation_PROBABILITY);

    std::random_device rd;
    std::mt19937       gen(rd());

    assignCharacteristic(piecesWithoutKings, PieceCharacteristic::Fuite, FuiteCount, gen);
    assignCharacteristic(piecesWithoutKings, PieceCharacteristic::RuptureAttention, RuptureAttentionCount, gen);
    assignCharacteristic(piecesWithoutKings, PieceCharacteristic::Dyscalculia, dyscalculiaCount, gen);
    assignCharacteristic(piecesWithoutKings, PieceCharacteristic::Mutation, MutationCount, gen);
}

Piece* Board::getPieceAt(int row, int col) const
{
    if (isValidPosition(row, col))
    {
        return board[row][col].get();
    }
    return nullptr;
}

bool Board::setPieceAt(int row, int col, std::unique_ptr<Piece> piece)
{
    if (isValidPosition(row, col))
    {
        board[row][col] = std::move(piece);
        return true;
    }
    return false;
}

bool Board::movePiece(int fromRow, int fromCol, int toRow, int toCol)
{
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol))
    {
        return false;
    }

    Piece* piece = getPieceAt(fromRow, fromCol);
    if (!isValidPosition(fromRow, fromCol) || !isValidPosition(toRow, toCol))
    {
        return false;
    }

    board[toRow][toCol] = std::move(board[fromRow][fromCol]);
    return true;
}

bool Board::isCellEmpty(int row, int col) const
{
    return getPieceAt(row, col) == nullptr;
}

bool Board::isValidPosition(int row, int col) const
{
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

std::vector<std::pair<int, int>> Board::getAllPiecesExceptKings() const
{
    std::vector<std::pair<int, int>> pieces;
    for (int row = 0; row < BOARD_SIZE; ++row)
    {
        for (int col = 0; col < BOARD_SIZE; ++col)
        {
            Piece* piece = getPieceAt(row, col);
            // Skip empty squares and kings
            if (piece && piece->getType() != PieceType::King)
            {
                pieces.push_back({row, col});
            }
        }
    }
    return pieces;
}

void Board::assignCharacteristic(const std::vector<std::pair<int, int>>& pieces, PieceCharacteristic characteristic, int count, std::mt19937& gen)
{
    std::vector<std::pair<int, int>> shuffledPieces = pieces;
    std::shuffle(shuffledPieces.begin(), shuffledPieces.end(), gen);

    for (int i = 0; i < count && i < static_cast<int>(shuffledPieces.size()); ++i)
    {
        auto [row, col] = shuffledPieces[i];
        board[row][col]->addCharacteristic(characteristic);
    }
}
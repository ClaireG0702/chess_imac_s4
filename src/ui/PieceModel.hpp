#pragma once

#include <map>
#include <memory>
#include <string>
#include "Mesh.hpp"

class PieceModel {
public:
    PieceModel() = default;
    ~PieceModel() = default;

    PieceModel(const PieceModel&) = delete;
    PieceModel& operator=(const PieceModel&) = delete;

    // Load piece models from a directory
    // Expects files like: pawn.glb, knight.glb, bishop.glb, rook.glb, queen.glb, king.glb
    bool loadFromDirectory(const std::string& directoryPath);

    // Load single piece model
    bool loadPiece(const std::string& pieceName, const std::string& filePath);

    // Get mesh for a specific piece type
    Mesh* getMesh(const std::string& pieceName);
    const Mesh* getMesh(const std::string& pieceName) const;

    // Check if a piece model is loaded
    bool hasPiece(const std::string& pieceName) const;

    // Get all loaded piece names
    std::vector<std::string> getPieceNames() const;

private:
    std::map<std::string, Mesh> m_pieces;
};

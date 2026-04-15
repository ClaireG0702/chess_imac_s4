#include "PieceModel.hpp"
#include "GLTFLoader.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

bool PieceModel::loadFromDirectory(const std::string& directoryPath)
{
    // List of piece types to load
    const std::vector<std::string> pieceTypes = {
        "pawn", "knight", "bishop", "rook", "queen", "king"
    };

    bool allLoaded = true;

    for (const auto& pieceType : pieceTypes)
    {
        std::string filePath = directoryPath + "/" + pieceType + ".glb";
        if (!loadPiece(pieceType, filePath))
        {
            std::cout << "Note: Could not load model for " << pieceType << std::endl;
            allLoaded = false;
        }
    }

    return allLoaded;
}

bool PieceModel::loadPiece(const std::string& pieceName, const std::string& filePath)
{
    // Check if file exists
    std::ifstream f(filePath);
    if (!f.good())
    {
        std::cerr << "File not found: " << filePath << std::endl;
        return false;
    }

    auto meshes = GLTFLoader::loadGLB(filePath);
    if (meshes.empty())
    {
        std::cerr << "Failed to load meshes from: " << filePath << std::endl;
        return false;
    }

    // Use the first mesh from the glb file
    m_pieces[pieceName] = std::move(meshes[0]);
    std::cout << "Loaded piece model: " << pieceName << std::endl;

    return true;
}

Mesh* PieceModel::getMesh(const std::string& pieceName)
{
    auto it = m_pieces.find(pieceName);
    if (it != m_pieces.end())
        return &(it->second);
    return nullptr;
}

const Mesh* PieceModel::getMesh(const std::string& pieceName) const
{
    auto it = m_pieces.find(pieceName);
    if (it != m_pieces.end())
        return &(it->second);
    return nullptr;
}

bool PieceModel::hasPiece(const std::string& pieceName) const
{
    return m_pieces.find(pieceName) != m_pieces.end();
}

std::vector<std::string> PieceModel::getPieceNames() const
{
    std::vector<std::string> names;
    for (const auto& pair : m_pieces)
        names.push_back(pair.first);
    return names;
}

#pragma once

#include <glm/glm.hpp>
#include <chrono>

struct PieceMovement {
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
    std::chrono::steady_clock::time_point startTime;
    float duration; // in seconds
    
    bool isActive() const {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now - startTime;
        return elapsed.count() < duration;
    }
    
    float getProgress() const {
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> elapsed = now - startTime;
        float progress = elapsed.count() / duration;
        return (progress > 1.0f) ? 1.0f : progress;
    }
};

class PieceAnimationManager {
private:
    PieceMovement m_currentMovement;
    bool m_hasActiveMovement;
    
public:
    PieceAnimationManager() : m_hasActiveMovement(false) {}
    
    void startMovement(int fromRow, int fromCol, int toRow, int toCol, float duration = 0.5f) {
        m_currentMovement.fromRow = fromRow;
        m_currentMovement.fromCol = fromCol;
        m_currentMovement.toRow = toRow;
        m_currentMovement.toCol = toCol;
        m_currentMovement.startTime = std::chrono::steady_clock::now();
        m_currentMovement.duration = duration;
        m_hasActiveMovement = true;
    }
    
    bool hasActiveMovement() const {
        if (!m_hasActiveMovement) return false;
        return m_currentMovement.isActive();
    }
    
    const PieceMovement& getActiveMovement() const {
        return m_currentMovement;
    }
    
    void update() {
        if (m_hasActiveMovement && !m_currentMovement.isActive()) {
            m_hasActiveMovement = false;
        }
    }
};

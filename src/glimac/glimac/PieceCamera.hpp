#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glimac {

class PieceCamera {
    private:
        glm::vec3 m_position;  // Camera position (at the piece location)
        float m_fAngleX;       // Horizontal rotation (around Y axis)
        float m_fAngleY;       // Vertical rotation (around X axis) - no limits

    public:
        PieceCamera(const glm::vec3& piecePosition = glm::vec3(0.f))
            : m_position(piecePosition), m_fAngleX(0.f), m_fAngleY(0.f) {}

        ~PieceCamera() = default;

        // Update piece position (when a new piece is selected)
        void setPiecePosition(const glm::vec3& piecePosition) {
            m_position = piecePosition;
        }

        // Rotate horizontally (left-right, around Y axis)
        void rotateLeft(float degrees) {
            m_fAngleX += degrees;
        }

        // Rotate vertically (up-down, around X axis) - NO LIMITS
        void rotateUp(float degrees) {
            m_fAngleY += degrees;
        }

        // Get view matrix - camera looking outward from piece position
        const glm::mat4 getViewMatrix() const {
            // Start with identity, then apply rotation
            glm::mat4 view = glm::mat4(1.f);
            view = glm::rotate(view, glm::radians(m_fAngleY), glm::vec3(1.f, 0.f, 0.f));
            view = glm::rotate(view, glm::radians(m_fAngleX), glm::vec3(0.f, 1.f, 0.f));
            // Then translate to piece position with a slight height offset
            glm::vec3 cameraPos = m_position + glm::vec3(0.f, 0.24f, 0.f);
            view = glm::translate(view, -cameraPos);
            return view;
        }
};

}

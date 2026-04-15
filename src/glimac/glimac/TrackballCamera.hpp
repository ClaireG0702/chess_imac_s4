#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glimac {

class TrackballCamera {
    private:
        float m_fDistance;
        float m_fAngleX;
        float m_fAngleY;

    public:
        TrackballCamera(float distance = 5.f, float angleX = 0.f, float angleY = 0.f): m_fDistance(distance), m_fAngleX(angleX), m_fAngleY(angleY) {}

        ~TrackballCamera() = default;

        void moveFront(float delta) {
            m_fDistance += delta;
        }

        void rotateLeft(float degrees) {
            m_fAngleX += degrees;
        }

        void rotateUp(float degrees) {
            m_fAngleY += degrees;
            // Clamp angle to prevent camera from going below the board
            // Limit vertical angle to prevent viewing from below the board plane
            const float MIN_ANGLE_Y = 10.0f;   // Keep camera above the board level
            const float MAX_ANGLE_Y = 85.0f;   // Up to nearly vertical
            if (m_fAngleY > MAX_ANGLE_Y) m_fAngleY = MAX_ANGLE_Y;
            if (m_fAngleY < MIN_ANGLE_Y) m_fAngleY = MIN_ANGLE_Y;
        }

        const glm::mat4 getViewMatrix() const {
            glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -m_fDistance));
            view = glm::rotate(view, glm::radians(m_fAngleY), glm::vec3(1.f, 0.f, 0.f));
            view = glm::rotate(view, glm::radians(m_fAngleX), glm::vec3(0.f, 1.f, 0.f));
            return view;
        }
};
}
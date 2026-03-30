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
        }

        const glm::mat4 getViewMatrix() const {
            glm::mat4 view = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -m_fDistance));
            view = glm::rotate(view, glm::radians(m_fAngleY), glm::vec3(1.f, 0.f, 0.f));
            view = glm::rotate(view, glm::radians(m_fAngleX), glm::vec3(0.f, 1.f, 0.f));
            return view;
        }
};
}
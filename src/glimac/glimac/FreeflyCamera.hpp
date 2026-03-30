#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include <glm/glm.hpp>

namespace glimac {
class FreeflyCamera {
    private:
        glm::vec3 m_Position;
        float m_fPhi;
        float m_fTheta;

        glm::vec3 m_FrontVector;
        glm::vec3 m_LeftVector;
        glm::vec3 m_UpVector;

        void computeDirectionVectors() {
            m_FrontVector = glm::vec3(
                cos(glm::radians(m_fTheta)) * sin(glm::radians(m_fPhi)),
                sin(glm::radians(m_fTheta)),
                cos(glm::radians(m_fTheta)) * cos(glm::radians(m_fPhi))
            );
            m_LeftVector = glm::vec3(
                sin(glm::radians(m_fPhi + 90.f)),
                0,
                cos(glm::radians(m_fPhi + 90.f))
            );
            m_UpVector = glm::cross(m_FrontVector, m_LeftVector);
        }

    public:
        FreeflyCamera(glm::vec3 position = glm::vec3(0.f), float phi = M_PI, float theta = 0.f): m_Position(position), m_fPhi(phi), m_fTheta(theta) {
            computeDirectionVectors();
        }

        ~FreeflyCamera() = default;

        void moveFront(float t) {
            m_Position += t * m_FrontVector;
        }

        void moveLeft(float t) {
            m_Position += t * m_LeftVector;
        }

        void moveUp(float delta) {
            m_Position += delta * m_UpVector;
        }

        void rotateLeft(float degrees) {
            m_fPhi += degrees;
            computeDirectionVectors();
        }

        void rotateUp(float degrees) {
            m_fTheta += degrees;
            computeDirectionVectors();
        }

        const glm::mat4 getViewMatrix() const {
            return glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
        }
};
}
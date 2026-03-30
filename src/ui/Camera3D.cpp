#include "Camera3D.hpp"

Camera3D::Camera3D()
    : m_viewportWidth(800), m_viewportHeight(800),
      m_position(glm::vec3(8.0f, 12.0f, 8.0f)),    // Isometric angle: elevated and back
      m_target(glm::vec3(3.5f, 0.0f, 3.5f)),       // Center of board (8x8 board centered at origin)
      m_up(glm::vec3(0.0f, 1.0f, 0.0f)),           // Up is Y axis
      m_fov(45.0f),
      m_near(0.1f),
      m_far(100.0f) {
}

void Camera3D::setViewport(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
}

glm::mat4 Camera3D::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera3D::getProjectionMatrix() const {
    float aspect = static_cast<float>(m_viewportWidth) / static_cast<float>(m_viewportHeight);
    return glm::perspective(glm::radians(m_fov), aspect, m_near, m_far);
}

glm::mat4 Camera3D::getViewProjectionMatrix() const {
    return getProjectionMatrix() * getViewMatrix();
}

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera3D {
public:
    Camera3D();

    // Set viewport dimensions for aspect ratio
    void setViewport(int width, int height);

    // Get view matrix (isometric, fixed angle)
    glm::mat4 getViewMatrix() const;

    // Get projection matrix (perspective to fit board)
    glm::mat4 getProjectionMatrix() const;

    // Get combined view-projection matrix
    glm::mat4 getViewProjectionMatrix() const;

private:
    int m_viewportWidth;
    int m_viewportHeight;
    
    // Isometric camera parameters (fixed)
    glm::vec3 m_position;      // Camera position
    glm::vec3 m_target;        // Look-at target (center of board)
    glm::vec3 m_up;            // Up vector
    
    float m_fov;               // Field of view (degrees)
    float m_near;              // Near clipping plane
    float m_far;               // Far clipping plane
};

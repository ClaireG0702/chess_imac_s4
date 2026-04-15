#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 fragPosition_ws;  // World space position
out vec3 fragNormal_ws;    // World space normal
out float fragCurrentPlayer;  // 0.0 for white player, 1.0 for black player

uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;
uniform float currentPlayer;  // 0.0 or 1.0

void main() {
    // Transform to world space
    fragPosition_ws = vec3(modelMatrix * vec4(position, 1.0));
    fragNormal_ws = mat3(transpose(inverse(modelMatrix))) * normal;
    fragCurrentPlayer = currentPlayer;
    
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(position, 1.0);
}

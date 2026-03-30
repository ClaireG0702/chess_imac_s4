#version 330 core
layout(location = 0) in vec3 position;

out vec3 vertexPosition;

uniform mat4 viewProjectionMatrix;

void main() {
    vertexPosition = position;
    gl_Position = viewProjectionMatrix * vec4(position, 1.0);
}

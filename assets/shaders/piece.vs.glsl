#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 fragNormal;
out vec3 fragPosition;

uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

void main() {
    fragPosition = vec3(modelMatrix * vec4(position, 1.0));
    fragNormal = mat3(transpose(inverse(modelMatrix))) * normal;
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(position, 1.0);
}

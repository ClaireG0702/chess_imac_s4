#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 vertexNormal;
out vec2 vTexCoord;

uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

void main() {
    vertexNormal = normal;
    vTexCoord = texCoord;
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(position, 1.0);
}

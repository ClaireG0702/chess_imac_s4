#version 330 core
in vec3 vertexNormal;
in vec2 vTexCoord;

out vec4 outColor;

uniform sampler2D albedoTexture;

void main() {
    // Sample texture using UV coordinates
    vec3 texColor = texture(albedoTexture, vTexCoord).rgb;
    outColor = vec4(texColor, 1.0);
}

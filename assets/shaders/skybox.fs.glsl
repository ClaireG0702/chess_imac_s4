#version 330 core
in vec3 vertexPosition;

out vec4 outColor;

uniform float time;

void main() {
    vec3 pos = normalize(vertexPosition);
    
    // Create abstract gradient pattern with animated elements
    vec3 color = vec3(0.0);
    
    // Base gradient: blue to purple transitioning
    color += mix(
        vec3(0.1, 0.1, 0.3),  // Dark blue
        vec3(0.3, 0.1, 0.5),  // Purple
        pos.y * 0.5 + 0.5
    );
    
    // Add some geometric patterns
    float pattern = abs(sin(pos.x * 5.0) * cos(pos.z * 5.0)) * 0.3;
    color += vec3(pattern * 0.2, pattern * 0.1, pattern * 0.3);
    
    // Some moving gradients
    float movement = sin(time * 0.1) * 0.2 + 0.3;
    color += vec3(0.0, movement * 0.1, 0.2);
    
    outColor = vec4(color, 1.0);
}

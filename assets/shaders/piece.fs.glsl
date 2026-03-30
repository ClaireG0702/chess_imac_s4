#version 330 core
in vec3 fragNormal;
in vec3 fragPosition;

out vec4 outColor;

uniform vec3 pieceColor;

void main() {
    // Simple Phong-like lighting
    vec3 lightPos = vec3(15.0, 20.0, 15.0);
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    
    // Ambient
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    // Diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(-fragPosition); // Simplified, camera at origin
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;
    
    vec3 result = (ambient + diffuse + specular) * pieceColor;
    outColor = vec4(result, 1.0);
}

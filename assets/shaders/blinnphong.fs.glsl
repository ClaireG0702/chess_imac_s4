#version 330 core
in vec3 fragPosition_ws;  // World space position
in vec3 fragNormal_ws;    // World space normal
in float fragCurrentPlayer;  // 0.0 for white player, 1.0 for black player

out vec4 outColor;

uniform vec3 pieceColor;
uniform vec3 cameraPosition;

vec3 blinnPhong(vec3 normal, vec3 position, vec3 viewDir, vec3 lightDir, vec3 lightIntensity, vec3 kd, vec3 ks, float shininess) {
    // Normalized vectors
    vec3 n = normalize(normal);
    vec3 wi = normalize(-lightDir);  // Direction toward light
    vec3 w0 = normalize(viewDir);     // Direction toward camera
    
    // Half vector for Blinn-Phong
    vec3 halfVector = normalize(w0 + wi);
    
    // Diffuse component
    vec3 diffuse = kd * lightIntensity * max(0.0, dot(n, wi));
    
    // Specular component
    vec3 specular = ks * lightIntensity * pow(max(0.0, dot(n, halfVector)), shininess);
    
    return diffuse + specular;
}

// Fill light: weaker light for shadow fill (no specular)
vec3 fillLight(vec3 normal, vec3 lightDir, vec3 lightIntensity, vec3 kd) {
    vec3 n = normalize(normal);
    vec3 wi = normalize(-lightDir);  // Direction toward light
    
    // Only diffuse component (no specular for fill light)
    vec3 diffuse = kd * lightIntensity * max(0.0, dot(n, wi));
    
    return diffuse;
}

void main() {
    // Normalize normal
    vec3 normal = normalize(fragNormal_ws);
    
    // View direction (from fragment to camera)
    vec3 viewDir = cameraPosition - fragPosition_ws;
    
    // Determine light direction based on current player
    // Light comes from directly above the board
    vec3 boardCenter = vec3(4.0, 0.0, 4.0);
    vec3 lightDirection;
    vec3 highlightColor;
    float shininess;
    
    if (fragCurrentPlayer < 0.5) {
        // White player's turn: Light from above
        lightDirection = normalize(vec3(0.0, 1.0, 0.0));  // Directly above
        highlightColor = vec3(0.7, 0.3, 0.9);  // Violet highlights
        shininess = 28.0;
    } else {
        // Black player's turn: Light from above
        lightDirection = normalize(vec3(0.0, 1.0, 0.0));  // Directly above
        highlightColor = vec3(0.2, 0.9, 0.3);  // Green highlights
        shininess = 24.0;
    }
    
    // Material properties
    vec3 kd = pieceColor;  // Diffuse color (piece color)
    vec3 ks = highlightColor;  // Specular color (player-based highlight)
    
    // Main light: White light intensity from above
    vec3 mainLightIntensity = vec3(1.0, 1.0, 1.0);
    
    // Calculate main Blinn-Phong lighting
    vec3 mainLitColor = blinnPhong(normal, fragPosition_ws, viewDir, lightDirection, mainLightIntensity, kd, ks, shininess);
    
    // Fill light: Weaker light from the side to fill shadows
    vec3 fillLightDir = normalize(vec3(1.0, 0.5, 1.0));  // From the side and slightly above
    vec3 fillLightIntensity = vec3(0.3, 0.3, 0.3);  // Weaker intensity (30% of main light)
    vec3 fillLitColor = fillLight(normal, fillLightDir, fillLightIntensity, kd);
    
    // Rim light: Light from camera direction for visible highlights from all angles
    vec3 rimLightDir = normalize(viewDir);  // Direction from camera
    float rimIntensity = 0.2;  // Subtle rim light
    vec3 rimLightColor = highlightColor;  // Use player's highlight color
    
    // Simple rim light calculation: strongest at grazing angles
    float rim = 1.0 - max(0.0, dot(normalize(fragNormal_ws), rimLightDir));
    rim = pow(rim, 2.0);  // Pow to make it more concentrated at edges
    vec3 rimLitColor = rimLightColor * rim * rimIntensity;
    
    // Add subtle ambient
    vec3 ambient = vec3(0.15, 0.15, 0.15) * pieceColor;
    
    // Combine main light, fill light, rim light, and ambient
    vec3 result = mainLitColor + fillLitColor + rimLitColor + ambient;
    outColor = vec4(result, 1.0);
}

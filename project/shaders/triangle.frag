#version 330 core
out vec4 fragColor;

in vec4 iColor;
in vec3 iPosition;
in vec3 iNormal;
in vec2 iTexCoord;

struct Light {
    vec4 Color;
    vec3 Position;
    float Intensity;
    float Brightness;
};

uniform Light lights[10];
uniform int numLights;

uniform vec3 cameraPosition;

void main() {
    const float ambient = 0.3f;
    const float specularLight = 0.1f;

    vec4 texColor = vec4(0.5);
    vec4 specColor = vec4(1);

    vec3 normal = normalize(iNormal);
    vec4 finalColor = vec4(0.0);

    for (int i = 0; i < numLights; i++) {
        vec3 lightDirection = normalize(lights[i].Position - iPosition);
        vec3 viewDirection = normalize(cameraPosition - iPosition);
        vec3 reflectionDirection = reflect(-lightDirection, normal);
        float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
        float specular = specAmount * specularLight;
        float diffuse = max(dot(normal, lightDirection), 0.0) * lights[i].Intensity;
        
        finalColor += ((diffuse + ambient) * texColor + specular * specColor.r) * lights[i].Color * lights[i].Brightness;
    }

    fragColor = finalColor;
}
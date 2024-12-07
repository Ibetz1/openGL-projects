#version 330 core
out vec4 fragColor;

in vec4 iColor;
in vec3 iPosition;
in vec3 iNormal;
in vec2 iTexCoord;

uniform sampler2D texture0;
uniform sampler2D specular0;
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

void main() {
    const float ambient = 0.1f;
    const float specularLight = 0.75f;

    vec4 texColor = texture(texture0, iTexCoord);
    vec4 specColor = texture(specular0, iTexCoord);

    vec3 normal = normalize(iNormal);
    vec3 lightDirection = normalize(lightPosition - iPosition);
    vec3 viewDirection = normalize(cameraPosition - iPosition);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
    float specular = specAmount * specularLight;
    float diffuse = max(dot(normal, lightDirection), 0.0);
    
    fragColor = ((diffuse + ambient) * texColor + specular * specColor.r) * lightColor;
}
#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec4 aColor;
layout(location = 3) in vec2 aTex;

uniform mat4 view;
uniform mat4 model;
uniform float texTileCount = 1;

out vec3 iNormal;
out vec3 iPosition;
out vec4 iColor;
out vec2 iTexCoord;

void main() {
    iColor = aColor;
    iTexCoord = aTex * texTileCount;
    iPosition = vec3(model * vec4(aPosition, 1.0));
    iNormal = aNormal;

    gl_Position = view * model * vec4(aPosition, 1.0);
}
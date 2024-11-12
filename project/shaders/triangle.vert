#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTex;

out vec3 color;

void main() {
    color = aColor;
    gl_Position = vec4(aPosition, 1.0);
}
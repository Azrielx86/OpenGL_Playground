#version 430

out vec4 FragColor;

uniform vec3 color = vec3(1.0f, 1.0f, 1.0f);

void main() {
    FragColor = vec4(color, 1.0f);
}
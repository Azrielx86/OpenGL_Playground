#version 430

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec2 uTexCoords;

void main() {
    uTexCoords = uv;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
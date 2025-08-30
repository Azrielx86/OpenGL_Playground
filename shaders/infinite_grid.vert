#version 430

layout (location = 0) in vec3 position;

uniform mat4 uVP;
uniform vec3 cameraPosition;
uniform float gridSize = 100.0f;

out vec3 WorldPos;
out vec3 CameraPosition;
out float GridSize;

void main()
{
    GridSize = gridSize;
    CameraPosition = cameraPosition;

    vec3 pos = position * gridSize;

    pos.x += cameraPosition.x;
    pos.z += cameraPosition.z;

    gl_Position = uVP * vec4(pos, 1.0f);

    WorldPos = pos;
}
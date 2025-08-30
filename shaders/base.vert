#version 430

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 2) in vec3 tangent;
layout (location = 2) in vec3 bitangent;

out vec2 uTexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec3 FragView;
out mat3 TBN;

void main() {
    uTexCoords = uv;
//    Normal = normal;
    Normal = mat3(transpose(inverse(model))) * normal;
    FragPos = vec3(model * vec4(position, 1.0));
    FragView = vec3(view * vec4(FragPos, 1.0));

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(model * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0f)));

    TBN = mat3(T, B, N);

    gl_Position = projection * vec4(FragView, 1.0);
}
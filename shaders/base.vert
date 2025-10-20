#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIds;
layout (location = 6) in vec4 weights;

out vec2 uTexCoords;
out vec3 Normal;
out vec3 FragPos;
out vec3 FragView;
out mat3 TBN;

const int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 bones[MAX_BONES];
uniform int numBones;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

void main() {
    uTexCoords = uv;

    mat4 boneTransform = mat4(1.0f);
    mat4 totalBoneTransform = mat4(0.0f);
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (boneIds[i] == -1) continue;
        if (weights[i] == 0.0f) continue;
        totalBoneTransform += bones[boneIds[i]] * weights[i];
    }
    boneTransform = totalBoneTransform;

    Normal = mat3(transpose(inverse(model * boneTransform))) * normal;

    vec4 worldPos = model * boneTransform * vec4(position, 1.0f);
    FragPos = worldPos.xyz;
    FragView = vec3(view * vec4(FragPos, 1.0));

    mat4 normalMatrix = transpose(inverse(model * boneTransform));
    vec3 T = normalize(vec3(normalMatrix * vec4(tangent, 0.0f)));
    vec3 B = normalize(vec3(normalMatrix * vec4(bitangent, 0.0f)));
    vec3 N = normalize(vec3(normalMatrix * vec4(normal, 0.0f)));

    TBN = mat3(T, B, N);

    gl_Position = projection * view * worldPos;
}
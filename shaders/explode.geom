#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec2 texCoords;
    vec3 FragView;
    mat3 TBN;
} gs_in[];

out vec2 uTexCoords;
out vec3 FragPos;
out vec3 FragView;
out mat3 TBN;

uniform float time;

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0f;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 0.0);
}

void main()
{
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    uTexCoords = gs_in[0].texCoords;
    TBN = gs_in[0].TBN;
    FragView = gs_in[0].FragView;
    EmitVertex();

    gl_Position = explode(gl_in[1].gl_Position, normal);
    uTexCoords = gs_in[1].texCoords;
    TBN = gs_in[1].TBN;
    FragView = gs_in[1].FragView;
    EmitVertex();

    gl_Position = explode(gl_in[2].gl_Position, normal);
    uTexCoords = gs_in[2].texCoords;
    TBN = gs_in[2].TBN;
    FragView = gs_in[2].FragView;
    EmitVertex();

    EndPrimitive();
}
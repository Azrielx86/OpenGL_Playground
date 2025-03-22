#version 430

layout (location = 0) out vec4 FragColor;

uniform float gridCellSize = 0.025;
uniform float gridMinPixels = 2.0;
uniform vec4 gridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
uniform vec4 gridColorThick = vec4(0.8, 0.8, 0.8, 1.0);

in vec3 WorldPos;
in vec3 CameraPosition;
in float GridSize;

vec2 saturatev2(vec2 value)
{
    return clamp(value, 0.0, 1.0);
}

float saturatef(float value)
{
    return clamp(value, 0.0, 1.0);
}

float max2(vec2 vector)
{
    return max(vector.x, vector.y);
}

float log10(float x)
{
    return log(x) / log(10.0);
}

float calc_lod_aplha(float cellSizeLod, vec2 dudv)
{
    vec2 modDudv = mod(WorldPos.xz, cellSizeLod) / dudv;
    return max2(vec2(1.0) - abs(saturatev2(modDudv) * 2.0 - vec2(1.0)));
}

void main()
{
    vec2 dvx = vec2(dFdx(WorldPos.x), dFdy(WorldPos.x));
    vec2 dvy = vec2(dFdx(WorldPos.z), dFdy(WorldPos.z));

    float lx = length(dvx);
    float ly = length(dvy);

    vec2 dudv = vec2(lx, ly);
    float l = length(dudv);

    float lod = max(0.0, log10(l * gridMinPixels / gridCellSize) + 1.0);

    float gridSizeLod0 = gridCellSize * pow(10.0, floor(lod));
    float gridSizeLod1 = gridSizeLod0 * 10.0;
    float gridSizeLod2 = gridSizeLod1 * 10.0;

    dudv *= 4.0;

//    vec2 modDudv = mod(WorldPos.xz, gridSizeLod0) / dudv;
//    float lod0a =  max2(vec2(1.0) - abs(saturatev2(modDudv) * 2.0 - vec2(1.0)));

    float lod0a = calc_lod_aplha(gridSizeLod0, dudv);
    float lod1a = calc_lod_aplha(gridSizeLod1, dudv);
    float lod2a = calc_lod_aplha(gridSizeLod2, dudv);

    float lodFade = fract(lod);

    vec4 Color;

    if (lod2a > 0.0)
    {
        Color = gridColorThick;
        Color.a *= lod2a;
    }
    else
    {
        if (lod1a > 0.0)
        {
            Color = mix(gridColorThick, gridColorThin, lodFade);
            Color.a *= lod1a;
        }
        else
        {
            Color = gridColorThin;
            Color.a *= (lod0a * (1.0 - lodFade));
        }
    }

    float falloff = (1.0 - saturatef(length(WorldPos.xz - CameraPosition.xz)) / GridSize);
    Color.a *= falloff;

    FragColor = Color;
}
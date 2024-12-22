#version 430

out vec4 outColor;
in vec2 uTexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

void main()
{
//    outColor = vec4(uTexCoords.xxx, 1.0f);
    outColor = texture(texture_diffuse1, uTexCoords);
}
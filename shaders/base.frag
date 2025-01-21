#version 430

out vec4 outColor;
in vec2 uTexCoords;
in vec3 FragPos;
in vec3 FragView;
in vec3 Normal;

uniform vec3 ambientLightColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_emissive;

void main()
{
    //    Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * ambientLightColor;
    //    Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    //    Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(FragView);
    vec3 reflectDir = reflect(-viewDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 specular = specularStrength * spec * lightColor;

    //    Total color
    vec4 totalColor = vec4((ambient + diffuse + specular), 1.0f);

    outColor = texture(texture_diffuse, uTexCoords) * totalColor;
}
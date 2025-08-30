#version 430

out vec4 outColor;
in vec2 uTexCoords;
in vec3 FragPos;
in vec3 FragView;
in vec3 Normal;
in mat3 TBN;

uniform vec3 ambientLightColor;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_emissive;
uniform sampler2D texture_normal;

struct Material {
    vec3 baseColor;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emissive;
    float shininess;
    bool textured;
};

struct PointLight {
    vec4 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float constant;
    float linear;
    float quadratic;

    bool isTurnedOn;
};

uniform int pointLightsSize;

layout (std430, binding = 3) buffer pointLights
{
    PointLight pointLightsData[];
};

uniform Material material;

vec4 CalcPointLight(PointLight light, vec3 normal)
{
    float dist = length(light.position.xyz - FragPos);
    float attenuation = 1.0f / (light.constant + (light.linear * dist) +
    (light.quadratic * (dist * dist)));

    //    Ambient
    vec3 ambient = light.ambient.rgb * texture(texture_diffuse, uTexCoords).rgb * attenuation;

    //    Diffuse
    vec3 lightDir = normalize(light.position.xyz - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse.rgb * diff * texture(texture_diffuse, uTexCoords).xyz * attenuation;

    //    Specular
    vec3 viewDir = normalize(FragView);
    vec3 reflectDir = reflect(-viewDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 1.0f), material.shininess);
    vec3 specular = light.specular.rgb * spec * texture(texture_specular, uTexCoords).xyz * attenuation;

    return vec4((ambient + diffuse + specular), texture(texture_diffuse, uTexCoords).a);
}

void main()
{
    vec3 normal = texture(texture_normal, uTexCoords).rgb;
    normal = normal * 2.0f - 1.0f;
    normal = normalize(TBN * normal);

    vec4 totalColor = vec4(0.0f);

    for (int i = 0; i < pointLightsSize; i++) {
        if (!pointLightsData[i].isTurnedOn) continue;
        totalColor += CalcPointLight(pointLightsData[i], normal);
    }

    outColor = totalColor;
}
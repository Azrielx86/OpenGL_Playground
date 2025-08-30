#version 430

out vec4 outColor;
in vec2 uTexCoords;
in vec3 FragPos;
in vec3 FragView;
in vec3 Normal;
in mat3 TBN;

uniform vec3 ambientLightColor;
uniform vec3 lightColor;
uniform vec3 lightPos;

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

uniform Material material;

void main()
{
    //    Ambient
    float ambientStrength = 0.0f;
    vec3 ambient = ambientStrength * ambientLightColor;

    vec3 normal = texture(texture_normal, uTexCoords).rgb;
    normal = normal * 2.0f - 1.0f;
    normal = normalize(TBN * normal);

    //    Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diff * texture(texture_diffuse, uTexCoords).xyz;

    //    Specular
    vec3 viewDir = normalize(FragView);
    vec3 reflectDir = reflect(-viewDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 1.0f), material.shininess);
    vec3 specular = lightColor * spec * texture(texture_specular, uTexCoords).xyz;
//    vec3 specular = lightColor * spec * vec3(0.0);

    //    Total color
    vec4 totalColor = vec4((ambient + diffuse + specular), texture(texture_diffuse, uTexCoords).a);

    outColor = totalColor;
//    outColor = texture(texture_diffuse, uTexCoords);
//    outColor = vec4(specular, 1.0);
}
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
    //    vec3 matDiff = material.diffuse;
    //
    //    if (material.textured)
    //    {
    //        matDiff = vec3(1.0);
    //    }

    //    Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * ambientLightColor;

    //    Diffuse
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = lightColor * diff * texture(texture_diffuse, uTexCoords).xyz;

    //    Specular
    vec3 viewDir = normalize(FragView);
    vec3 reflectDir = reflect(-viewDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 1.0f), material.shininess);
    vec3 specular = lightColor * spec * texture(texture_specular, uTexCoords).xyz;

    //    Total color
    vec4 totalColor = vec4((ambient + diffuse + specular), 1.0f);

    //    if (material.textured)
    //    {
    outColor = totalColor;
    //    }
    //    else
    //    {
    //        outColor = vec4(material.diffuse.xyz, 1.0);
    //    }
}
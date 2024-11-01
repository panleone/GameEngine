#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 fragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 eyePos;

uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient*vec3(texture(material.diffuse, TexCoord));

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffusion = light.diffuse*(diff*vec3(texture(material.diffuse, TexCoord)));

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(eyePos - fragPos);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0),material.shininess);
    vec3 specular = light.specular*(vec3(texture(material.specular, TexCoord)) * spec);

    vec3 result = ambient + diffusion + specular;
    FragColor = vec4(result, 1.0);
}
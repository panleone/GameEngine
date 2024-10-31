#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

out vec4 FragColor;

uniform vec3 eyePos;

uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient*material.ambient;

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffusion = light.diffuse*(diff*material.diffuse);

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(eyePos - fragPos);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0),material.shininess);
    vec3 specular = light.specular*(material.specular * spec);

    vec3 result = ambient + diffusion + specular;

    FragColor = vec4(result, 1.0);
}
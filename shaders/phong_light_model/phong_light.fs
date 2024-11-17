#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

in vec3 normal;
in vec3 fragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 eyePos;

uniform Material material;
#define N_MAX_LIGHTS 10
uniform Light lights[N_MAX_LIGHTS];
uniform int nLights = 0;

vec3 CalcLightColor(Light light, vec3 normal, vec3 fragPos, vec3 eyePos){
    vec3 diffuseTexel = vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 specularTexel = vec3(texture(material.texture_specular1, TexCoord));

    vec3 ambient = light.ambient * diffuseTexel;

    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffusion = light.diffuse * diff * diffuseTexel;

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(eyePos - fragPos);
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), 32.0);
    vec3 specular = light.specular * specularTexel * spec;

    float d = length(fragPos - light.position);
    vec3 dVec = vec3(1, d, d*d);
    float attenuation = 1.0/dot(light.attenuation, dVec);

    return (ambient + diffusion + specular) * attenuation;
}
void main()
{
    vec3 color = vec3(0.0);
    for (int i = 0; i < nLights; i++){
        color += CalcLightColor(lights[i], normal, fragPos, eyePos);
    }
    FragColor = vec4(color, 1.0);
}
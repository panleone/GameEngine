#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

struct Light {
    // For point light the lightVector is the position of the light source. Fourth component is 1.0
    // For directional lights the lightVector is the direction of the light rays. Fourth component is 0.0
    vec4 lightVector;

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
#define N_MAX_LIGHTS 100
uniform Light lights[N_MAX_LIGHTS];
uniform int nLights = 0;

uniform bool blinnCorrection = true;

vec3 CalcLightInternal(Light light, vec3 normal, vec3 eyePos, vec3 diffuseTexel, vec3 specularTexel, vec3 viewDir, vec3 lightDir){
    vec3 ambient = light.ambient * diffuseTexel;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffusion = light.diffuse * diff * diffuseTexel;

    float spec = 0;
    if(blinnCorrection){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(halfwayDir, normal), 0.0), 32.0);
    } else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(reflectDir, viewDir), 0.0), 32.0);
    }
    vec3 specular = light.specular * specularTexel * spec;

    return (ambient + diffusion + specular);
}

vec3 CalcPointLightColor(Light light, vec3 normal, vec3 fragPos, vec3 eyePos, vec3 diffuseTexel, vec3 specularTexel, vec3 viewDir){
    vec3 lightPos = light.lightVector.xyz;
    vec3 lightDir = normalize(lightPos - fragPos);
    vec3 partial = CalcLightInternal(light, normal, eyePos, diffuseTexel, specularTexel, viewDir, lightDir);

    float d = length(fragPos - lightPos);
    vec3 dVec = vec3(1, d, d*d);
    float attenuation = 1.0/dot(light.attenuation, dVec);

    return partial * attenuation;
}

vec3 CalcDirLightColor(Light light, vec3 normal, vec3 eyePos, vec3 diffuseTexel, vec3 specularTexel, vec3 viewDir) {
    vec3 lightDir = normalize(-light.lightVector.xyz);
    return CalcLightInternal(light, normal, eyePos, diffuseTexel, specularTexel, viewDir, lightDir);
}

void main()
{
    vec4 diffuseVec = vec4(texture(material.texture_diffuse1, TexCoord));
    vec3 specularTexel = vec3(texture(material.texture_specular1, TexCoord));
    vec3 diffuseTexel = diffuseVec.xyz;
    float alpha = diffuseVec.w;
    vec3 viewDir = normalize(eyePos - fragPos);

    vec3 color = vec3(0.0);
    for (int i = 0; i < nLights; i++){
        if(lights[i].lightVector.w > 0.99) {
            color += CalcPointLightColor(lights[i], normal, fragPos, eyePos, diffuseTexel, specularTexel, viewDir);
        } else {
            color += CalcDirLightColor(lights[i], normal, eyePos, diffuseTexel, specularTexel, viewDir);
        }
    }
    FragColor = vec4(color, alpha);
}
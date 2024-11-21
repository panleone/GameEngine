#version 330 core

out vec4 FragColor;
uniform sampler2D postProcessing;


in VS_OUT {
    vec2 TexCoord;
} fs_in;

void main()
{
    FragColor = texture(postProcessing, fs_in.TexCoord);
}
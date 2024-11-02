#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 mMatrix;

out VS_OUT {
    vec2 TexCoord;
} vs_out;

void main()
{
    gl_Position = mMatrix*vec4(aPos.x, aPos.y, aPos.z, 1.0);
    vs_out.TexCoord = aTexCoord;
}

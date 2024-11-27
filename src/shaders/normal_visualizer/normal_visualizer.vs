#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 vmMatrix;

void main()
{
    gl_Position = vmMatrix*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

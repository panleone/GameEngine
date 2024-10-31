#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 pMatrix;

const float SCALING = 0.4;

void GenerateLine(int index, vec3 normal)
{
    gl_Position = pMatrix * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = pMatrix * (gl_in[index].gl_Position +
                                vec4(normal, 0.0) * SCALING);
    EmitVertex();
    EndPrimitive();
}

void main() {
    vec3 d1 = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 d2 = vec3(gl_in[0].gl_Position) - vec3(gl_in[2].gl_Position);
    vec3 normal = 0.2*normalize(cross(d1, d2));

    GenerateLine(0, normal);
    GenerateLine(1, normal);
    GenerateLine(2, normal);
}
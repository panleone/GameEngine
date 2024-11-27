#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT {
    vec2 TexCoord;
} gs_in[];

out vec3 normal;
out vec3 fragPos;
out vec2 TexCoord;

uniform mat4 pvMatrix;

void main() {
    vec3 d1 = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 d2 = vec3(gl_in[0].gl_Position) - vec3(gl_in[2].gl_Position);
    normal = normalize(cross(d1, d2));

    gl_Position = pvMatrix*gl_in[0].gl_Position;
    fragPos = gl_in[0].gl_Position.xyz;
    TexCoord = gs_in[0].TexCoord;
    EmitVertex();

    gl_Position = pvMatrix*gl_in[1].gl_Position;
    fragPos = gl_in[1].gl_Position.xyz;
    TexCoord = gs_in[1].TexCoord;
    EmitVertex();

    gl_Position = pvMatrix*gl_in[2].gl_Position;
    fragPos = gl_in[2].gl_Position.xyz;
    TexCoord = gs_in[2].TexCoord;
    EmitVertex();
    EndPrimitive();
}
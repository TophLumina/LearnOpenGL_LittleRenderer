#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

const float len = 0.1;

in VS_OUT {
    vec3 normal;
} gs_in[];

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * len;
    EmitVertex();
    EndPrimitive();
}

void main() {
    GenerateLine(0);
    // I wander if i can make the normal root on the center of the triangle, but failed to do it.
}
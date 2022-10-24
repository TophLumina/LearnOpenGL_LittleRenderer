#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

uniform mat4 Shadow_Matrices[6];

out vec4 FragPos;

void main() {
    for (int face = 0; face < 6; ++face) {
        gl_Layer = face; // bulit_in vars
        // Vertices in a Triangle
        for (int vex = 0; vex < 3; ++vex) {
            FragPos = gl_in[vex].gl_Position;
            gl_Position = Shadow_Matrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}
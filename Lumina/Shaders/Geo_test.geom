#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT {
    vec3 col;
} gs_in[];

out vec3 fs_col;

void pattenbulid(vec4 center) {
    fs_col = gs_in[0].col;

    gl_Position = center + vec4(0.2, 0.2, 0.0, 0.0);
    EmitVertex();

    gl_Position = center + vec4(-0.2, 0.2, 0.0, 0.0);
    EmitVertex();

    gl_Position = center + vec4(0.2, -0.2, 0.0, 0.0);
    EmitVertex();

    gl_Position = center + vec4(-0.2, -0.2, 0.0, 0.0);
    EmitVertex();

    // Before Construct new Primitive Update its Data
    fs_col = vec3(1.0, 1.0, 1.0);
    gl_Position = center + vec4(0.0, -0.4, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}

void main() {    
    pattenbulid(gl_in[0].gl_Position);
}
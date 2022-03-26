# version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

const float level = 0.5;

in VS_OUT {
    vec3 normal;
    vec3 fragPos;
    vec2 texCoords;
} gs_in[];

out vec2 texCoords;

vec3 CaculateNormal() {
    vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);

    return normalize(cross(a, b));
};

vec4 Explode(vec4 pos, vec3 norm) {
    return pos + level * vec4(norm ,0.0);
};

void main() {
    vec3 norm = CaculateNormal();

    gl_Position = Explode(gl_in[0].gl_Position, norm);
    texCoords = gs_in[0].texCoords;
    EmitVertex();

    gl_Position = Explode(gl_in[1].gl_Position, norm);
    texCoords = gs_in[1].texCoords;
    EmitVertex();

    gl_Position = Explode(gl_in[2].gl_Position, norm);
    texCoords = gs_in[2].texCoords;
    EmitVertex();

    EndPrimitive();
}
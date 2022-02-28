# version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoords;

void main() {
    normal = mat3(transpose(inverse(model * view))) * aNormal;
    fragPos = vec3(view * model * vec4(aPosition, 1.0));
    texCoords = aTexCoords;
    gl_Position = projection * vec4(fragPos, 1.0);
}
# version 330 core

uniform vec3 light_col;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

void main() {
    FragColor = vec4(light_col, 1.0);
    float bright = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (bright > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
}
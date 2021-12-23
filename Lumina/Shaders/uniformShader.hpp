#pragma once

const char* VertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* FragShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 dynamic;\n"
"void main()\n"
"{\n"
"	FragColor = dynamic;\n"
"}\n\0";
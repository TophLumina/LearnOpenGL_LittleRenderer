#pragma once

const char* VertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char* OrangeFragShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor=vec4(1.0f,0.5f,0.2f,1.0f);\n"
"}\n";

const char* GreenFragShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor=vec4(0.5f,1.0f,0.2f,1.0f);\n"
"}\n";
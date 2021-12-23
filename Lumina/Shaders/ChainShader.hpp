#pragma once

const char* VertexShaderSource = "#version 330 core\n"
//Shaders are indepandent Programs linked down the PipLine
//use "in" and "out" to transfer data to the next Shader (by name matching)
"layout (location = 0) in vec3 aPos;\n"
"out vec4 vertexColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"   vertexColor = vec4(aPos, 1.0);\n"
"}\0";

const char* FragShaderSource = "#version 330 core\n"
"in vec4 vertexColor;\n"//now we pass the vertexColor from the perivous Shader
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vertexColor;\n"
"}\n";
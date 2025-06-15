#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <string>
#include <glad/glad.h>

unsigned int createShaderProgram(const char* vertexPath, const char* fragmentPath);
unsigned int createShaderFromSource(const char* vertexSource, const char* fragmentSource);
std::string loadShaderSource(const char* filepath);
void checkShaderCompilation(unsigned int shader, const char* type);
void checkProgramLinking(unsigned int program);

#endif // SHADER_UTILS_H
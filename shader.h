#ifndef SHADER_H
#define SHADER_H

int loadShader(GLchar **shaderStr, char *fileName);
GLuint createShader(GLenum shaderType, const GLchar *shaderStr);
GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader);

#endif // SHADER_H

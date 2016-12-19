#include <stdio.h>
#include <stdlib.h>
#include <GLES2/gl2.h>

int loadShader(GLchar **shaderStr, char *fileName)
{
	uint filesize;	
	FILE *file = fopen(fileName,"r");

	fseek(file,0,SEEK_END);
	filesize=ftell(file);
	rewind(file);	
	*shaderStr = (GLchar*)malloc(filesize+1);
	fread(*shaderStr,1,filesize,file);
	(*shaderStr)[filesize]='\0';
	fclose(file);

	return 0;
}

GLuint createShader(GLenum shaderType, const GLchar *shaderStr)
{
	GLuint shader;

	shader = glCreateShader(shaderType);
	glShaderSource(shader,1,&shaderStr,NULL);
	glCompileShader(shader);		

	return shader;
}

GLuint createShaderProgram(GLuint vertexShader, GLuint fragmentShader)
{
	GLuint shaderProgram;
	
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	return shaderProgram;
}

#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

typedef struct
{
	GLfloat *vertices;
	GLuint verticesLen, verticesSize;
	GLushort *indices;
	GLuint indicesLen, indicesSize;
	GLuint vboID, iboID;
	GLuint shaderProgram;
	GLuint vertexHandle;
	
} Object;

void createVBO(GLenum bufferType, GLuint *bufferID, GLuint bufferSize, GLvoid *buffer);
void initObject(Object *obj);

#endif

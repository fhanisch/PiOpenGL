#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

typedef struct
{
	GLfloat *vertices;
	GLuint verticesLen, verticesSize;
	GLfloat *u;
	GLuint uLen, uSize;
	GLushort *indices;
	GLuint indicesLen, indicesSize;
	GLuint vboID, iboID;
	GLuint shaderProgram;
	GLuint vertexHandle;
	GLint mProjHandle, mModelHandle, colorHandle;
	Matrix4 mProj, mModel;
	Vector4 color;
    GLenum renderMode;
} Object;

void createVBO(GLenum bufferType, GLuint *bufferID, GLuint bufferSize, GLvoid *buffer);
void initObject(Object *obj, GLuint shaderProgram, char *fileName);
void drawObject(Object *o);
status loadModel(Object *o, char *fileName);

#endif

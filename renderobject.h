#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

typedef struct
{
	GLfloat *vertices;
	GLuint verticesLen, verticesSize;
	GLfloat *u;
	GLuint uLen, uSize;
	GLfloat *texCoords;
	GLuint texCoordsLen, texCoordsSize;
	GLushort *indices;
	GLuint indicesLen, indicesSize;
	GLuint vboID, iboID, uboID, tcoID, texID;
	GLboolean isVBO, isUBO, isTCO, isTex;
	GLuint shaderProgram;
	GLuint vertexHandle, uHandle, texCoordsHandle;
	GLint mProjHandle, mModelHandle, colorHandle, samplerHandle;
	Matrix4 mProj, mModel;
	Vector4 color;
    GLenum renderMode;
    GLenum textureIndex;
} Object;

void createVBO(GLenum bufferType, GLuint *bufferID, GLuint bufferSize, GLvoid *buffer);
void initObject(Object *obj, GLuint shaderProgram, char *fileName);
void drawObject(Object *o);
status loadModel(Object *o, char *fileName);

#endif // RENDEROBJECT_H

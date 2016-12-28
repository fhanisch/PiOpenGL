#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

typedef enum {FROM_FILE=0b00000001, MESH_GRID=0b00000010, OBJ_3D=0b00000100} ObjFlags;

typedef struct
{
	GLfloat *vertices;
	GLuint verticesLen, verticesSize;
	GLfloat *u, *v;
	GLuint uLen, vLen, uSize, vSize;
	GLfloat *texCoords;
	GLuint texCoordsLen, texCoordsSize;
	GLushort *indices;
	GLuint indicesLen, indicesSize;
	GLuint vboID, iboID, uID, vID, tcoID, texID;
	GLboolean isVBO, isU, isV, isTCO, isTex;
	GLuint shaderProgram;
	GLuint vertexHandle, uHandle, vHandle, texCoordsHandle;
	GLint mProjHandle, mViewHandle, mModelHandle, colorHandle, samplerHandle;
	Matrix4 mProj, mModel;
	pMatrix4 mView;
	Vector4 color;
	GLenum renderMode;
	GLenum textureIndex;
} Object;

void createVBO(GLenum bufferType, GLuint *bufferID, GLuint bufferSize, GLvoid *buffer);
void initObject(Object *obj, pMatrix4 mView, GLuint shaderProgram, char *fileName, ObjFlags flags);
void drawObject(Object *o);
status loadModel(Object *o, char *fileName);

#endif // RENDEROBJECT_H

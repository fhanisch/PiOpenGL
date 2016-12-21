#include <stdlib.h>
#include <GLES2/gl2.h>
#include "matrix.h"
#include "renderobject.h"

void createVBO(GLenum bufferType, GLuint *bufferID, GLuint bufferSize, GLvoid *buffer)
{	
	glGenBuffers(1,bufferID);
	glBindBuffer(bufferType, *bufferID);
	glBufferData(bufferType, bufferSize, buffer, GL_STATIC_DRAW);
}

void initObject(Object *obj, GLuint shaderProgram)
{
	obj->verticesLen = 3*3;
	obj->verticesSize = obj->verticesLen * sizeof(GLfloat);
	obj->vertices = malloc(obj->verticesSize);
	obj->indicesLen = 3;
	obj->indicesSize = obj->indicesLen * sizeof(GLushort);
	obj->indices = malloc(obj->indicesSize);
	
	obj->vertices[0]=-1.0f;obj->vertices[1]=-1.0f;obj->vertices[2]=0.0f;
	obj->vertices[3]= 0.0f;obj->vertices[4]= 1.0f;obj->vertices[5]=0.0f;
	obj->vertices[6]= 1.0f;obj->vertices[7]=-1.0f;obj->vertices[8]=0.0f;
	
	obj->indices[0]=0;obj->indices[1]=1;obj->indices[2]=2;
	
	obj->mProj = identity();
	obj->color = getGreen();
	
	obj->shaderProgram = shaderProgram;
	obj->vertexHandle = glGetAttribLocation(obj->shaderProgram,"vertex");
	obj->mProjHandle = glGetUniformLocation(obj->shaderProgram,"mProj");
	obj->colorHandle = glGetUniformLocation(obj->shaderProgram,"color");
	
	createVBO(GL_ARRAY_BUFFER, &obj->vboID, obj->verticesSize, obj->vertices);
	createVBO(GL_ELEMENT_ARRAY_BUFFER, &obj->iboID, obj->indicesSize, obj->indices);
}

void drawObject(Object *o)
{
	glUseProgram(o->shaderProgram);
	glUniformMatrix4fv(o->mProjHandle,1,GL_FALSE,(GLfloat*)&o->mProj);
	glUniform4fv(o->colorHandle,1, (GLfloat*)&o->color);
	glBindBuffer(GL_ARRAY_BUFFER, o->vboID);
	glEnableVertexAttribArray(o->vertexHandle);
	glVertexAttribPointer(o->vertexHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawElements(GL_TRIANGLES, o->indicesLen, GL_UNSIGNED_SHORT, 0);
}

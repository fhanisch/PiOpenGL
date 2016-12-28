#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLES2/gl2.h>
#include "PiOpenGL.h"
#include "matrix.h"
#include "renderobject.h"

void createVBO(GLenum bufferType, GLuint *bufferID, GLuint bufferSize, GLvoid *buffer)
{
	glGenBuffers(1,bufferID);
	glBindBuffer(bufferType, *bufferID);
	glBufferData(bufferType, bufferSize, buffer, GL_STATIC_DRAW);
}

void initObject(Object *obj, pMatrix4 mView, GLuint shaderProgram, char *fileName, ObjFlags flags)
{
    obj->isVBO = GL_FALSE;
    obj->isU = GL_FALSE;
    obj->isV = GL_FALSE;
    obj->isTCO = GL_FALSE;
    obj->isTex = GL_FALSE;

    if ((flags&FROM_FILE)==FROM_FILE)
		loadModel(obj,fileName);
	else if ((flags&MESH_GRID)==MESH_GRID)
	{
		createMeshGrid(&obj->u,&obj->v,&obj->uSize,&obj->vSize, 50, 50);
		obj->isU = GL_TRUE;
		obj->isV = GL_TRUE;
		createMeshGridIndices(&obj->indices,&obj->indicesLen,&obj->indicesSize,50,50);
	}

    obj->mProj = scaleMatrix(identity(),vec3((float)SCREEN_HEIGHT/(float)SCREEN_WIDTH,1.0f,1.0f));
    obj->mView = mView;
    obj->mModel = identity();
    obj->shaderProgram = shaderProgram;

    obj->mProjHandle = glGetUniformLocation(obj->shaderProgram,"mProj");
    obj->mViewHandle = glGetUniformLocation(obj->shaderProgram,"mView");
    obj->mModelHandle = glGetUniformLocation(obj->shaderProgram,"mModel");
    obj->mTransInvModelViewHandle = glGetUniformLocation(obj->shaderProgram,"mTransInvModelView");
    obj->colorHandle = glGetUniformLocation(obj->shaderProgram,"color");
    obj->samplerHandle = glGetUniformLocation(obj->shaderProgram,"samp");

	///Wichtig: Möglichst keine identischen Buffer mehrfach generieren!
	if (obj->isVBO)
	{
		obj->vertexHandle = glGetAttribLocation(obj->shaderProgram,"vertex");
		createVBO(GL_ARRAY_BUFFER, &obj->vboID, obj->verticesSize, obj->vertices);
		free(obj->vertices);
	}
	if (obj->isU)
	{
		obj->uHandle = glGetAttribLocation(obj->shaderProgram,"u");
		createVBO(GL_ARRAY_BUFFER, &obj->uID, obj->uSize, obj->u);
		free(obj->u);
	}
	if (obj->isV)
	{
		obj->vHandle = glGetAttribLocation(obj->shaderProgram,"v");
		createVBO(GL_ARRAY_BUFFER, &obj->vID, obj->vSize, obj->v);
		free(obj->v);
	}
	if (obj->isTCO)
	{
		obj->texCoordsHandle = glGetAttribLocation(obj->shaderProgram,"texcoord");
		createVBO(GL_ARRAY_BUFFER, &obj->tcoID, obj->texCoordsSize, obj->texCoords);
		free(obj->texCoords);
	}
	createVBO(GL_ELEMENT_ARRAY_BUFFER, &obj->iboID, obj->indicesSize, obj->indices);
	free(obj->indices);
}

void drawObject(Object *o)
{
	glUseProgram(o->shaderProgram);

	glUniformMatrix4fv(o->mProjHandle,1,GL_FALSE,(GLfloat*)pTmpTranspose(&o->mProj)); // Matrizen für Shader müssen transponiert werden
	glUniformMatrix4fv(o->mViewHandle,1,GL_FALSE,(GLfloat*)pTmpTranspose(o->mView)); // Matrizen für Shader müssen transponiert werden
	glUniformMatrix4fv(o->mModelHandle,1,GL_FALSE,(GLfloat*)pTmpTranspose(&o->mModel)); // Matrizen für Shader müssen transponiert werden
	if(o->mTransInvModelViewHandle>=0) glUniformMatrix4fv(o->mTransInvModelViewHandle,1,GL_FALSE,(GLfloat*)&o->mTransInvModelView);
	glUniform4fv(o->colorHandle,1, (GLfloat*)&o->color);
	if (o->samplerHandle>=0) glUniform1i(o->samplerHandle,0);

	if (o->isVBO)
	{
		glBindBuffer(GL_ARRAY_BUFFER, o->vboID);
		glEnableVertexAttribArray(o->vertexHandle);
		glVertexAttribPointer(o->vertexHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (o->isU)
	{
		glBindBuffer(GL_ARRAY_BUFFER, o->uID);
		glEnableVertexAttribArray(o->uHandle);
		glVertexAttribPointer(o->uHandle, 1, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (o->isV)
	{
		glBindBuffer(GL_ARRAY_BUFFER, o->vID);
		glEnableVertexAttribArray(o->vHandle);
		glVertexAttribPointer(o->vHandle, 1, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (o->isTCO)
	{
		glBindBuffer(GL_ARRAY_BUFFER, o->tcoID);
		glEnableVertexAttribArray(o->texCoordsHandle);
		glVertexAttribPointer(o->texCoordsHandle, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	if (o->isTex)
    {
		glBindTexture(GL_TEXTURE_2D,o->texID);
    }
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o->iboID);
	glDrawElements(o->renderMode, o->indicesLen, GL_UNSIGNED_SHORT, 0);
}

status loadModel(Object *o, char *fileName)
{
    FILE *file;
    uint filesize;
    uint bufPtr=0;
    uint i;
    char *buffer;
    float start, step;
    uint count;

    file = fopen(fileName,"r");
    fseek(file,0,SEEK_END);
    filesize=ftell(file);
    rewind(file);
    buffer = (char*)malloc(filesize)+1;
    fread(buffer,1,filesize,file);
    buffer[filesize] = '\0';

    while(bufPtr<filesize)
    {
        if(!strncmp(buffer+bufPtr,"vertices",strlen("vertices")))
        {
            printf("%s\n","vertices");
            bufPtr+=strlen("vertices")+1;
            sscanf(buffer+bufPtr,"%d",&o->verticesLen);
            printf("Anzahl: %d\n",o->verticesLen);
            o->verticesSize = 3 * o->verticesLen * sizeof(GLfloat);
            o->vertices = malloc(o->verticesSize);
            o->isVBO = GL_TRUE;
            while(buffer[bufPtr]!='\n') bufPtr++;
            bufPtr++;
            for(i=0;i<o->verticesLen;i++)
            {
                sscanf(buffer+bufPtr,"%f %f %f",o->vertices+3*i,o->vertices+3*i+1,o->vertices+3*i+2);
                printf("%4.1f %4.1f %4.1f\n",o->vertices[3*i],o->vertices[3*i+1],o->vertices[3*i+2]);
                while(buffer[bufPtr]!='\n') bufPtr++;
                bufPtr++;
            }
            continue;
        }
        else if(!strncmp(buffer+bufPtr,"indices",strlen("indices")))
        {
            printf("%s\n","indices");
            bufPtr+=strlen("indices")+1;
            sscanf(buffer+bufPtr,"%d",&o->indicesLen);
            printf("Anzahl: %d\n",o->indicesLen);
            o->indicesSize = o->indicesLen * sizeof(GLushort);
            o->indices = malloc(o->indicesSize);
            while(buffer[bufPtr]!='\n') bufPtr++;
            bufPtr++;
            for(i=0;i<o->indicesLen;i++)
            {
                sscanf(buffer+bufPtr,"%hu",o->indices+i);
                printf("%d ",o->indices[i]);
                while(buffer[bufPtr]!=' ' && buffer[bufPtr]!='\n') bufPtr++;
                bufPtr++;
            }
            printf("\n");
            continue;
        }
        else if(!strncmp(buffer+bufPtr,"u",1))
        {
            bufPtr+=2;
            sscanf(buffer+bufPtr,"%f %f %u", &start, &step, &count);
            printf("%0.2f %0.2f %u\n", start, step, count);
            o->u = vecnf(start, step, count);
            o->uLen = count;
            o->uSize = count*sizeof(GLfloat);
            o->isU = GL_TRUE;
            o->indices = vecns(0,count);
            o->indicesLen = count;
            o->indicesSize = count * sizeof(GLushort);
            while(buffer[bufPtr]!='\n') bufPtr++;
            bufPtr++;
            continue;
        }
        else if(!strncmp(buffer+bufPtr,"texcoords",strlen("texcoords")))
        {
            printf("%s\n","texcoords");
            bufPtr+=strlen("texcoords")+1;
            sscanf(buffer+bufPtr,"%d",&o->texCoordsLen);
            printf("Anzahl: %d\n",o->texCoordsLen);
            o->texCoordsSize = 2 * o->texCoordsLen * sizeof(GLfloat);
            o->texCoords = malloc(o->texCoordsSize);
            o->isTCO = GL_TRUE;
            while(buffer[bufPtr]!='\n') bufPtr++;
            bufPtr++;
            for(i=0;i<o->texCoordsLen;i++)
            {
                sscanf(buffer+bufPtr,"%f %f",o->texCoords+2*i,o->texCoords+2*i+1);
                printf("%4.1f %4.1f\n",o->texCoords[2*i],o->texCoords[2*i+1]);
                while(buffer[bufPtr]!='\n') bufPtr++;
                bufPtr++;
            }
            continue;
        }
        bufPtr++;
    }

    fclose(file);

    return ok;
}

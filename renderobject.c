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

void initObject(Object *obj, GLuint shaderProgram, char *fileName)
{
    loadModel(obj,fileName);

    obj->mProj = scaleMatrix(identity(),vec3((float)SCREEN_HEIGHT/(float)SCREEN_WIDTH,1.0f,1.0f));
    obj->mProj = transpose(obj->mProj);
    obj->shaderProgram = shaderProgram;
    obj->vertexHandle = glGetAttribLocation(obj->shaderProgram,"vertex");
    obj->mProjHandle = glGetUniformLocation(obj->shaderProgram,"mProj");
    obj->mModelHandle = glGetUniformLocation(obj->shaderProgram,"mModel");
    obj->colorHandle = glGetUniformLocation(obj->shaderProgram,"color");

    createVBO(GL_ARRAY_BUFFER, &obj->vboID, obj->verticesSize, obj->vertices);
    createVBO(GL_ELEMENT_ARRAY_BUFFER, &obj->iboID, obj->indicesSize, obj->indices);
}

void drawObject(Object *o)
{
	glUseProgram(o->shaderProgram);
	glUniformMatrix4fv(o->mProjHandle,1,GL_FALSE,(GLfloat*)&o->mProj);
	glUniformMatrix4fv(o->mModelHandle,1,GL_FALSE,(GLfloat*)&o->mModel);
	glUniform4fv(o->colorHandle,1, (GLfloat*)&o->color);
	glBindBuffer(GL_ARRAY_BUFFER, o->vboID);
	glEnableVertexAttribArray(o->vertexHandle);
	glVertexAttribPointer(o->vertexHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawElements(o->renderMode, o->indicesLen, GL_UNSIGNED_SHORT, 0);
}

status loadModel(Object *o, char *fileName)
{
    FILE *file;
    uint filesize;
    uint bufPtr=0;
    uint i;
    char *buffer;
    char str[16];

    file = fopen(fileName,"r");
    fseek(file,0,SEEK_END);
    filesize=ftell(file);
    rewind(file);
    buffer = (char*)malloc(filesize)+1;
    fread(buffer,1,filesize,file);
    buffer[filesize] = '\0';

    while(bufPtr<filesize)
    {
        sscanf(buffer+bufPtr,"%s",str);
        if(!strcmp(str,"vertices"))
        {
            printf("%s\n","vertices");
            bufPtr+=strlen("vertices")+1;
            sscanf(buffer+bufPtr,"%d",&o->verticesLen);
            printf("Anzahl: %d\n",o->verticesLen);
            o->verticesSize = 3 * o->verticesLen * sizeof(GLfloat);
            o->vertices = malloc(o->verticesSize);
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
        else if(!strcmp(str,"indices"))
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
        bufPtr++;
    }

    fclose(file);

    return ok;
}

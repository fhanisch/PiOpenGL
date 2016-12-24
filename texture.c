#include <stdio.h>
#include <stdlib.h>
#include <GLES2/gl2.h>
#include "texture.h"

void initTexture(Texture *tex, GLenum texindex, char *fileName)
{
	tex->textureIndex = texindex;
	tex->fileName = fileName;
	loadTexture(tex);
	glActiveTexture(tex->textureIndex);
	genTexture(tex);
}

int loadTexture(Texture *tex)
{
	char signature[2];
	unsigned short farbtiefe;
	unsigned int offset;
	unsigned int bytePtr=0;
	unsigned int x,y;

	FILE *file = fopen(tex->fileName,"r");
	fseek(file,0,SEEK_SET);
	fread(signature,2,1,file);
	fseek(file,28,SEEK_SET);
	fread(&farbtiefe,2,1,file);
	fseek(file,18,SEEK_SET);
	fread(&tex->xSize,4,1,file);
	fread(&tex->ySize,4,1,file);
	tex->texture = (GLubyte*)malloc(tex->xSize*tex->ySize*4);
	fseek(file,10,SEEK_SET);
	fread(&offset,4,1,file);
	fseek(file,offset,SEEK_SET);
	for (y=0;y<tex->ySize;y++)
	{
		for (x=0;x<tex->xSize;x++)
		{
			fread(tex->texture+bytePtr+2,1,1,file);
			fread(tex->texture+bytePtr+1,1,1,file);
			fread(tex->texture+bytePtr,1,1,file);
			tex->texture[bytePtr+3] = 0xff;
			bytePtr+=4;
		}
		fseek(file,tex->xSize%4,SEEK_CUR);
	}

	fclose(file);
	return 0;
}

void genTexture(Texture *tex)
{
	glGenTextures(1, &tex->id);
	glBindTexture(GL_TEXTURE_2D,tex->id);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tex->xSize, tex->ySize, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->texture);
}

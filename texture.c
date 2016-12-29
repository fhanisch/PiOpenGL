#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLES2/gl2.h>
#include "texture.h"

void display_buffer_hex(unsigned char *buffer, unsigned size)
{
	unsigned int width=30;
	unsigned i, j;

	printf("%s","\x1B[31m");
	for (i=0; i<size; i+=width) {
		printf("\n  %08x  ", i);
		for(j=0; j<width; j++)
		{
			printf("%02x", buffer[i+j]);
			printf(" ");
		}
		printf(" ");
		for(j=0; j<width; j++)
		{
			if ((buffer[i+j] < 32) || (buffer[i+j] > 126))
			{
				printf(".");
			}
			else
			{
				printf("%c", buffer[i+j]);
			}
		}
	}
	printf("%s","\x1B[0m");
	printf("\n\n" );
}

void convertTexture(GLubyte *texture, void *pixels, uint xSize, uint ySize)
{
	uint i, j, k, ptr=0;

	for (i=ySize-1;i>=1;i--)
	{
		k=i*xSize*3;
		for (j=0;j<xSize;j++)
		{
            if (((GLubyte*)pixels)[ptr]>0)
				texture[k] = 255;
			else
				texture[k] = 0;
			texture[k+1] = 0;
			texture[k+2] = 0;
			k+=3;
			ptr++;
		}
	}
}

void setPixel(GLubyte *tex, int xSize, int x, int y)
{
	GLubyte value = 255;
	uint pixelPtr;

	pixelPtr = (y*xSize+x)*3;
	tex[pixelPtr] = value;
	tex[pixelPtr+1] = value;
	tex[pixelPtr+2] = value;
}

void loadCharMap(GLubyte *tex, int xSize)
{
	FILE *file;
	GLubyte charMap[128*16];
	GLubyte test=1;
	uint character, zeile, bit;
	uint charPtr=0;

	file = fopen("res/font.bin","rb");
	printf("Size Of CharMap: %i\n",sizeof(charMap));
	fread(charMap,sizeof(charMap),1,file);
	fclose(file);
	printf("Font geladen\n");
	for (character=0;character<20;character++)
	{
		for (zeile=0;zeile<16;zeile++)
		{
			for (bit=0;bit<8;bit++)
			{
				if (charMap[charPtr+48*16]>>bit & test)
				{
					setPixel(tex,xSize,bit+character*8,15-zeile);
				}
			}
			charPtr++;
		}
		printf("charPtr %d\n",charPtr);
	}
}

void initTexture(Texture *tex, GLenum texindex, GLenum format, char *fileName)
{
	tex->textureIndex = texindex;
	tex->pixelFormat = format;

	if (!strcmp(fileName,"res/font.bin"))
	{
		printf("Test font\n");
		tex->xSize = 128 * 8;
		tex->ySize = 16;
		tex->texture = malloc(tex->xSize*tex->ySize*3);
		memset(tex->texture, 0, tex->xSize*tex->ySize*3);
		loadCharMap(tex->texture,tex->xSize);
	}
	else
	{
		tex->fileName = fileName;
		loadTexture(tex);
	}

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
	tex->texture = (GLubyte*)malloc(tex->xSize*tex->ySize*3);
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
			bytePtr+=3;
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
	glTexImage2D( GL_TEXTURE_2D, 0, tex->pixelFormat, tex->xSize, tex->ySize, 0, tex->pixelFormat, GL_UNSIGNED_BYTE, tex->texture);
}

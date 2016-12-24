#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct
{
	GLuint id;
	GLenum textureIndex;
	GLubyte *texture;
	uint xSize, ySize;
	GLchar *fileName;
} Texture;

void initTexture(Texture *tex, GLenum texindex, char *fileName);
int loadTexture(Texture *tex);
void genTexture(Texture *tex);

#endif // TEXTURE_H

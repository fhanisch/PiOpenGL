#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct
{
	GLuint id;
	GLenum textureIndex;
	GLubyte *texture;
	uint xSize, ySize;
	GLchar *fileName;
	GLenum pixelFormat;
} Texture;

void initTexture(Texture *tex, GLenum texindex, GLenum format, char *fileName, SDL_Surface *font);
int loadTexture(Texture *tex);
void genTexture(Texture *tex);

#endif // TEXTURE_H

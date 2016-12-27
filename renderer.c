#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL_ttf.h>
#include "types.h"
#include "PiOpenGL.h"
#include "shader.h"
#include "matrix.h"
#include "renderobject.h"
#include "texture.h"

#define VS_GENERIC "shader/generic.vs"
#define VS_GENERIC_TEX "shader/generic_tex.vs"
#define VS_CIRCLE "shader/circle.vs"
#define FS_GENERIC "shader/generic.fs"
#define FS_GENERIC_TEX "shader/generic_tex.fs"
#define FS_GENERIC_TEXT "shader/generic_text.fs"

#define TEXT_OUT "Das ist das Haus vom Nikolaus !!!"

static CUBE_STATE_T *state = NULL;
static USB_DEV *usb_dev;
static bool f1 = FALSE;
static Object triangle, rect, cross, circle, stern, background, boden, txtBlock, cube;

void initOpenGL(CUBE_STATE_T *s, USB_DEV *dev)
{
	const GLubyte *vendor, *renderer, *oglVersion, *glslVersion;

	state=s;
	usb_dev = dev;
	vendor = glGetString(GL_VENDOR);
	renderer = glGetString(GL_RENDERER);
	oglVersion = glGetString(GL_VERSION);
	glslVersion	= glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("Vendor: %s\n",vendor);
	printf("Renderer: %s\n",renderer);
	printf("OpenGL Version: %s\n",oglVersion);
	printf("GLSL Version: %s\n",glslVersion);

	if (f1)
	{
		//Transparenz
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		//Transparenz
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);

		glClearDepthf(1.0f);
	}

	glClearColor(0.0f,0.0f,0.0f,1.0f); // Transparenz möglich
}

void initRenderScene()
{
	GLchar *vsGenericStr, *vsGenericTexStr, *vsCircleStr, *fsGenericStr, *fsGenericTexStr, *fsGenericTextStr;
	GLuint generic_vs, generictex_vs, circle_vs, generic_fs, generictex_fs, generictext_fs, generic_sp, generictex_sp, generictext_sp, circle_sp;
	Texture tex, fliessen, text;
	TTF_Font *font;
	SDL_Surface *message;
	SDL_PixelFormat *format;
	SDL_Color textColor = { 255, 255, 255 };

	//Create Shader
	loadShader(&vsGenericStr, VS_GENERIC);
	loadShader(&fsGenericStr, FS_GENERIC);
	loadShader(&vsGenericTexStr, VS_GENERIC_TEX);
	loadShader(&fsGenericTexStr, FS_GENERIC_TEX);
	loadShader(&fsGenericTextStr, FS_GENERIC_TEXT);
	loadShader(&vsCircleStr, VS_CIRCLE);

	generic_vs = createShader(GL_VERTEX_SHADER, vsGenericStr);
	generictex_vs = createShader(GL_VERTEX_SHADER, vsGenericTexStr);
	circle_vs = createShader(GL_VERTEX_SHADER, vsCircleStr);

	generic_fs = createShader(GL_FRAGMENT_SHADER, fsGenericStr);
	generictex_fs = createShader(GL_FRAGMENT_SHADER, fsGenericTexStr);
	generictext_fs = createShader(GL_FRAGMENT_SHADER, fsGenericTextStr);

	generic_sp = createShaderProgram(generic_vs, generic_fs);
	generictex_sp = createShaderProgram(generictex_vs, generictex_fs);
	generictext_sp = createShaderProgram(generictex_vs, generictext_fs);
	circle_sp = createShaderProgram(circle_vs, generic_fs);

	free(vsGenericStr);
	free(fsGenericStr);
	free(vsGenericTexStr);
	free(fsGenericTexStr);
	free(fsGenericTextStr);
	free(vsCircleStr);

	//Font
	font = TTF_OpenFont( "res/Ubuntu-B.ttf", 32 );
	message = TTF_RenderText_Solid( font, TEXT_OUT, textColor );
	printf("w: %i\n",message->w);
	printf("h: %i\n",message->h);
	format = message->format;
	printf("BitsPerPixel: %i\n",format->BitsPerPixel);
	printf("BytesPerPixel: %i\n",format->BytesPerPixel);

	//Texture
	initTexture(&tex, GL_TEXTURE0, GL_RGB, "res/green_scratches.bmp", NULL);
	initTexture(&fliessen, GL_TEXTURE0, GL_RGB, "res/fliessen.bmp",NULL);
	initTexture(&text, GL_TEXTURE0, GL_RGB, NULL, message);
	printf("Texuture xSize = %u\n",tex.xSize);
	printf("Texuture ySize = %u\n",tex.ySize);
	printf("Texture ID = %d\n", tex.id);
	printf("Texture Index = %d\n", tex.textureIndex);

	//Triangle
	initObject(&triangle, generic_sp, "res/triangle.geo");
	triangle.renderMode = GL_TRIANGLES;
	triangle.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,1.0f));
	triangle.mModel = translateMatrix(triangle.mModel,vec3(-1.0f,0.5f,0.0f));
	triangle.color = getColor(0.0f, 1.0f, 1.0f, 1.0f);

	//Rectangle
	initObject(&rect, generic_sp, "res/rectangle.geo");
	rect.renderMode = GL_TRIANGLE_STRIP;
	rect.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,1.0f));
	rect.mModel = translateMatrix(rect.mModel,vec3(1.0,-0.5,0.0));
	rect.color = getColor(1.0f,1.0f,0.0f,1.0f);
	rect.isTCO = GL_FALSE;

	//Cross
	initObject(&cross, generic_sp, "res/cross.geo");
	cross.renderMode = GL_LINES;
	cross.mProj = identity();
	cross.mModel = identity();
	cross.color = getColor(1.0,0.0,0.0,1.0);

	//Circle
	initObject(&circle, circle_sp, "res/circle.geo");
	circle.renderMode = GL_TRIANGLE_FAN;
	circle.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,1.0f));
	circle.mModel = translateMatrix(circle.mModel,vec3(1.0,0.5,0.0));
	circle.color = getColor(0.0,1.0,0.0,1.0);

	//Stern
	initObject(&stern, generic_sp, "res/stern.geo");
	stern.renderMode = GL_TRIANGLES;
	stern.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,1.0f));
	stern.mModel = translateMatrix(stern.mModel,vec3(-1.0,-0.5,0.0));
	stern.color = getColor(0.8,0.0,0.8,1.0);

	//background
	initObject(&background, generictex_sp, "res/rectangle.geo");
	background.renderMode = GL_TRIANGLE_STRIP;
	background.mProj = identity();
	background.mModel = translateMatrix(identity(),vec3(0.0,0.0,0.0));
	background.isTex = GL_TRUE;
	background.texID = tex.id;

	//Boden
	boden = background;
	boden.texID = fliessen.id;
    boden.mProj = rect.mProj;
    boden.mModel = scaleMatrix(identity(),vec3(0.4,0.4,1.0));

    //Text Block
    initObject(&txtBlock, generictext_sp, "res/rectangle.geo");
    txtBlock.renderMode = GL_TRIANGLE_STRIP;
    txtBlock.texID = text.id;
    txtBlock.isTex = GL_TRUE;
    txtBlock.mModel = scaleMatrix(identity(),vec3(0.5,0.05,1.0));
    txtBlock.mModel = translateMatrix(txtBlock.mModel,vec3(-0.4,0.8,0.0));

    //cube
    initObject(&cube,generictex_sp,"res/cube.geo");
    cube.renderMode = GL_TRIANGLES;
	cube.mProj = getFrustum(0.25*(GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,0.25,0.5,100.0);
	cube.mModel = translateMatrix(identity(), vec3(0.0,0.0,-5.0));
	cube.texID = fliessen.id;
	cube.isTex = GL_TRUE;
}

void renderLoop()
{
	status ret;
	bool quit=FALSE;
	unsigned char keybuf[8];
	unsigned char gamectrlbuf[8];
	int transferred;
	uint i,j;

	memset(keybuf, 0, sizeof(keybuf));
	memset(gamectrlbuf, 128, sizeof(gamectrlbuf));

	while(!quit)
	{
		if (f1)
		{
			glClear(GL_COLOR_BUFFER_BIT);

			drawObject(&background);
			drawObject(&boden);
			drawObject(&triangle);
			drawObject(&rect);
			drawObject(&circle);
			drawObject(&stern);
			drawObject(&cross);
			drawObject(&txtBlock);

			eglSwapBuffers(state->display, state->surface);

			ret = libusb_bulk_transfer(usb_dev->hid_keyboard,ENDPOINT_ADDRESS,keybuf,5,&transferred,1);
			if (ret==0 || ret==-7)
			{
				if (keybuf[2]==0x4f || keybuf[3]==0x4f) translatePtrMatrix(&rect.mModel,pTmpVec3(0.05,0.0,0.0));
				if (keybuf[2]==0x50 || keybuf[3]==0x50) translatePtrMatrix(&rect.mModel,pTmpVec3(-0.05,0.0,0.0));
				if (keybuf[2]==0x51 || keybuf[3]==0x51) translatePtrMatrix(&rect.mModel,pTmpVec3(0.0,-0.05,0.0));
				if (keybuf[2]==0x52 || keybuf[3]==0x52) translatePtrMatrix(&rect.mModel,pTmpVec3(0.0,+0.05,0.0));
				if (keybuf[2]==0x29) quit=TRUE;
			}
			else
			{
				printf("Transfer Error: %d\n",ret);
				quit=TRUE;
			}
			ret = libusb_bulk_transfer(usb_dev->hid_gamecontroller,ENDPOINT_ADDRESS,gamectrlbuf,5,&transferred,1);
			if (ret==0 || ret==-7)
			{
				translatePtrMatrix(&circle.mModel, pTmpVec3((GLfloat)gamectrlbuf[0]/256.0-0.5,-(GLfloat)gamectrlbuf[1]/256.0+0.5,0.0));
				stern.mModel = multPtrMatrix(&stern.mModel, pTmpGetRotZ((GLfloat)gamectrlbuf[2]/256.0-0.5));
				stern.mModel = multPtrMatrix(pTmpGetRotZ((GLfloat)gamectrlbuf[3]/256.0-0.5), &stern.mModel);
			}
			else
			{
				printf("Transfer Error: %d\n",ret);
				quit=TRUE;
			}
		}
		else
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			for (i=0;i<10;i++)
			{
				for (j=0;j<10;j++)
				{
					setPtrPosition(&cube.mModel,pTmpVec3((float)i*5.0,0.0,-(float)j*5.0));
					drawObject(&cube);
				}
			}

			eglSwapBuffers(state->display, state->surface);

			ret = libusb_bulk_transfer(usb_dev->hid_keyboard,ENDPOINT_ADDRESS,keybuf,5,&transferred,1);
			if (ret==0 || ret==-7)
			{
				//if (keybuf[2]==0x4f || keybuf[3]==0x4f) translatePtrMatrix(&cube.mView,pTmpVec3(0.0,0.0,0.5));
				//if (keybuf[2]==0x50 || keybuf[3]==0x50) translatePtrMatrix(&cube.mView,pTmpVec3(0.0,0.0,-0.0));
				if (keybuf[2]==0x1a || keybuf[3]==0x1a) translatePtrMatrix(&cube.mView,pTmpVec3(0.0,0.0,0.05));
				if (keybuf[2]==0x16 || keybuf[3]==0x16) translatePtrMatrix(&cube.mView,pTmpVec3(0.0,0.0,-0.05));
				if (keybuf[2]==0x04 || keybuf[3]==0x04) translatePtrMatrix(&cube.mView,pTmpVec3(0.05,0.0,0.0));
				if (keybuf[2]==0x07 || keybuf[3]==0x07) translatePtrMatrix(&cube.mView,pTmpVec3(-0.05,0.0,0.0));
				if (keybuf[2]==0x4f || keybuf[3]==0x4f) cube.mView = multPtrMatrix(pTmpGetRotY(0.05),&cube.mView);
				if (keybuf[2]==0x50 || keybuf[3]==0x50) cube.mView = multPtrMatrix(pTmpGetRotY(-0.05),&cube.mView);
				if (keybuf[2]==0x29) quit=TRUE;
			}
			else
			{
				printf("Transfer Error: %d\n",ret);
				quit=TRUE;
			}
			ret = libusb_bulk_transfer(usb_dev->hid_gamecontroller,ENDPOINT_ADDRESS,gamectrlbuf,5,&transferred,1);
			if (ret==0 || ret==-7)
			{
				translatePtrMatrix(&cube.mView, pTmpVec3(0.0, 0.0, -((GLfloat)gamectrlbuf[1]/256.0-0.5)));
				translatePtrMatrix(&cube.mView, pTmpVec3(-((GLfloat)gamectrlbuf[0]/256.0-0.5), 0.0, 0.0));
				cube.mView = multPtrMatrix(pTmpGetRotY((GLfloat)gamectrlbuf[2]/256.0-0.5), &cube.mView);
			}
			else
			{
				printf("Transfer Error: %d\n",ret);
				quit=TRUE;
			}
		}
	}
}

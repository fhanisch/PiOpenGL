#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libusb-1.0/libusb.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "types.h"
#include "PiOpenGL.h"
#include "shader.h"
#include "matrix.h"
#include "renderobject.h"
#include "texture.h"

#define VS_SHADERNAME "generic.vs"
#define VS_CIRCLE "circle.vs"
#define FS_SHADERNAME "generic.fs"

static CUBE_STATE_T *state = NULL;
static USB_DEV *usb_dev;
static Object triangle, rect, cross, circle, stern;
static Texture tex;

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

	glClearColor(0.0f,0.0f,1.0f,0.0f);
}

void initRenderScene()
{
	GLchar *vsStr, *vsCircleStr, *fsStr;
	GLuint generic_vs, circle_vs, generic_fs, generic_sp, circle_sp;

	loadShader(&vsStr, VS_SHADERNAME);
	loadShader(&fsStr, FS_SHADERNAME);
	loadShader(&vsCircleStr, VS_CIRCLE);
	printf("%s\n",vsStr);
	printf("%s\n",fsStr);
	printf("%s\n",vsCircleStr);

	generic_vs = createShader(GL_VERTEX_SHADER, vsStr);
	printf("Vertex Shader: %d\n",generic_vs);

	circle_vs = createShader(GL_VERTEX_SHADER, vsCircleStr);
	printf("Circle Shader: %d\n",circle_vs);

	generic_fs = createShader(GL_FRAGMENT_SHADER, fsStr);
	printf("Fragment Shader: %d\n",generic_fs);

	generic_sp = createShaderProgram(generic_vs, generic_fs);
	printf("Shader Program Generic: %d\n",generic_sp);

	circle_sp = createShaderProgram(circle_vs, generic_fs);
	printf("Shader Program Circle: %d\n",circle_sp);

	free(vsStr);
	free(fsStr);
	free(vsCircleStr);

	//Texture
	initTexture(&tex, GL_TEXTURE0, "res/green_scratches.bmp");
	printf("Texuture xSize = %u\n",tex.xSize);
	printf("Texuture ySize = %u\n",tex.ySize);
	printf("Texture ID = %d\n", tex.id);
	printf("Texture Index = %d\n", tex.textureIndex);
	printf("Texture Index = %d\n", GL_TEXTURE0);

	//Triangle
	initObject(&triangle, generic_sp, "res/triangle.geo");
	triangle.renderMode = GL_TRIANGLES;
	triangle.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,0.4f));
	triangle.mModel = translateMatrix(triangle.mModel,vec3(-1.0f,0.5f,0.0f));
	triangle.color = getColor(0.0f, 1.0f, 1.0f, 1.0f);

	//Rectangle
	initObject(&rect, generic_sp, "res/rectangle.geo");
	rect.renderMode = GL_TRIANGLE_STRIP;
	rect.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,0.4f));
	rect.mModel = translateMatrix(rect.mModel,vec3(1.0,-0.5,0.0));
	rect.color = getColor(1.0f,1.0f,0.0f,1.0f);

	//Cross
	initObject(&cross, generic_sp, "res/cross.geo");
	cross.renderMode = GL_LINES;
	cross.mModel = scaleMatrix(identity(),vec3((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,1.0f,1.0f));
	cross.color = getColor(1.0,0.0,0.0,1.0);

	//Circle
	initObject(&circle, circle_sp, "res/circle.geo");
	circle.renderMode = GL_TRIANGLE_FAN;
	circle.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,0.4f));
	circle.mModel = translateMatrix(circle.mModel,vec3(1.0,0.5,0.0));
	circle.color = getColor(0.0,1.0,0.0,1.0);

	//Stern
	initObject(&stern, generic_sp, "res/stern.geo");
	stern.renderMode = GL_TRIANGLES;
	stern.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,0.4f));
	stern.mModel = translateMatrix(stern.mModel,vec3(-1.0,-0.5,0.0));
	stern.color = getColor(0.8,0.0,0.8,1.0);
}

void renderLoop()
{
	status ret;
	bool quit=FALSE;
	unsigned char keybuf[8];
	unsigned char gamectrlbuf[8];
	int transferred;

	memset(keybuf, 0, sizeof(keybuf));
	memset(gamectrlbuf, 128, sizeof(gamectrlbuf));

	while(!quit)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		drawObject(&triangle);
		drawObject(&rect);
		drawObject(&cross);
		drawObject(&circle);
		drawObject(&stern);

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
			translatePtrMatrix(&circle.mModel, pTmpVec3((GLfloat)gamectrlbuf[0]/255.0-0.5,-(GLfloat)gamectrlbuf[1]/255.0+0.5,0.0));
			stern.mModel = multPtrMatrix(&stern.mModel, pTmpGetRotZ((GLfloat)gamectrlbuf[2]/255.0-0.5));
			stern.mModel = multPtrMatrix(pTmpGetRotZ((GLfloat)gamectrlbuf[3]/255.0-0.5), &stern.mModel);
		}
		else
		{
			printf("Transfer Error: %d\n",ret);
			quit=TRUE;
		}
	}
}

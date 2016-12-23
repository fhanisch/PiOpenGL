#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "types_egl.h"
#include "PiOpenGL.h"
#include "shader.h"
#include "matrix.h"
#include "renderobject.h"

#define VS_SHADERNAME "generic.vs"
#define FS_SHADERNAME "generic.fs"

static CUBE_STATE_T *state = NULL;
static libusb_device_handle *usb_dev = NULL;
static Object triangle, rect, cross;

void initOpenGL(CUBE_STATE_T *s, libusb_device_handle *dev)
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

	glClearColor(1.0f,0.0f,1.0f,1.0f);
}

void initRenderScene()
{
	GLchar *vsStr, *fsStr;
	GLuint generic_vs, generic_fs, generic_sp;

	loadShader(&vsStr, VS_SHADERNAME);
	loadShader(&fsStr, FS_SHADERNAME);
	printf("%s\n",vsStr);
	printf("%s\n",fsStr);

	generic_vs = createShader(GL_VERTEX_SHADER, vsStr);
	printf("Vertex Shader: %d\n",generic_vs);

	generic_fs = createShader(GL_FRAGMENT_SHADER, fsStr);
	printf("Fragment Shader: %d\n",generic_fs);

	generic_sp = createShaderProgram(generic_vs, generic_fs);
	printf("Shader Program: %d\n",generic_sp);

	free(vsStr);
	free(fsStr);

    //Triangle
	initObject(&triangle, generic_sp, "res/triangle.geo");
	triangle.renderMode = GL_TRIANGLES;
	triangle.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,0.4f));
	triangle.mModel = translateMatrix(triangle.mModel,vec3(-1.0f,0.5f,0.0f));
	triangle.mModel = transpose(triangle.mModel); // Matrizen für Shader müssen transponiert werden
	triangle.color = getColor(0.0f, 1.0f, 1.0f, 1.0f);

    //Rectangle
	initObject(&rect, generic_sp, "res/rectangle.geo");
	rect.renderMode = GL_TRIANGLE_STRIP;
	rect.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,0.4f));
	rect.mModel = translateMatrix(rect.mModel,vec3(1.0,-0.5,0.0));
	rect.mModel = transpose(rect.mModel);
	rect.color = getColor(1.0f,1.0f,0.0f,1.0f);

	//Cross
	initObject(&cross, generic_sp, "res/cross.geo");
	cross.renderMode = GL_LINES;
	cross.mModel = scaleMatrix(identity(),vec3((float)SCREEN_WIDTH/(float)SCREEN_HEIGHT,1.0f,1.0f));
	cross.color = getColor(1.0,0.0,0.0,1.0);

	printf("vboID: %d\n",triangle.vboID);
	printf("iboID: %d\n",triangle.iboID);
}

void renderLoop()
{
	status ret;
	bool quit=FALSE;
	unsigned char rcvbuf[8];
	int transferred;

	while(!quit)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		drawObject(&triangle);
		drawObject(&rect);
		drawObject(&cross);

		eglSwapBuffers(state->display, state->surface);

		ret = libusb_bulk_transfer(usb_dev,ENDPOINT_ADDRESS,rcvbuf,5,&transferred,0);
		if (ret==0)
		{
			if (rcvbuf[2]==0x4f) glClearColor(1.0f,0.0f,1.0f,1.0f);
			if (rcvbuf[2]==0x50) glClearColor(1.0f,0.0f,0.0f,1.0f);
			if (rcvbuf[2]==0x51) glClearColor(0.0f,1.0f,0.0f,1.0f);
			if (rcvbuf[2]==0x52) glClearColor(0.0f,0.0f,1.0f,1.0f);
			if (rcvbuf[2]==0x29) quit=TRUE;
		}
		else
		{
			printf("Transfer Error: %d\n",ret);
			quit=TRUE;
		}
	}
}

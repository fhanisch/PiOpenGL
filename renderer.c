#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "PiOpenGL.h"
#include "shader.h"
#include "renderobject.h"

#define VS_SHADERNAME "generic.vs"
#define FS_SHADERNAME "generic.fs"

static CUBE_STATE_T *state = NULL;
static libusb_device_handle *usb_dev = NULL;
static Object triangle;

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
	
	initObject(&triangle);
	triangle.shaderProgram = generic_sp;
	triangle.vertexHandle = glGetAttribLocation(triangle.shaderProgram,"vertex");
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
		glUseProgram(triangle.shaderProgram);
		glEnableVertexAttribArray(triangle.vertexHandle);
		glBindBuffer(GL_ARRAY_BUFFER, triangle.vboID);
		glVertexAttribPointer(triangle.vertexHandle, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawElements(GL_TRIANGLES, triangle.indicesLen, GL_UNSIGNED_SHORT, 0);
		
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

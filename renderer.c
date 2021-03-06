#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libusb-1.0/libusb.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <cblas.h>
#include <lapacke.h>
#include "types.h"
#include "PiOpenGL.h"
#include "shader.h"
#include "matrix.h"
#include "renderobject.h"
#include "texture.h"

#define VS_GENERIC "shader/generic.vs"
#define VS_GENERIC_TEX "shader/generic_tex.vs"
#define VS_CIRCLE "shader/circle.vs"
#define VS_SPHERE "shader/sphere.vs"
#define FS_GENERIC "shader/generic.fs"
#define FS_GENERIC_TEX "shader/generic_tex.fs"
#define FS_GENERIC_TEXT "shader/generic_text.fs"
#define FS_ADS_PER_FRAGMENT "shader/ads_per_fragment.fs"

#define TEXT_OUT "Das ist das Haus vom Nikolaus !!!"

typedef struct
{
	float x,y,z;
	float theta, phi, psi;
} Camera;


static CUBE_STATE_T *state = NULL;
static USB_DEV *usb_dev;
static bool f1 = FALSE;
static bool f2 = TRUE;
static Object triangle, rect, cross, circle, stern, background, boden, txtBlock,
		cube, cube2, sphere, plane;
static pMatrix4 mView;

static GLuint charOffset[128];

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

	//Transparenz
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	if (f2)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearDepthf(1.0f);
	}

	glClearColor(0.0f,0.0f,0.0f,1.0f); // Transparenz m�glich
}

void initRenderScene()
{
	GLchar *vsGenericStr, *vsGenericTexStr, *vsCircleStr, *vsSphereStr, *fsGenericStr, *fsGenericTexStr, *fsGenericTextStr, *fsADSperFragStr;
	GLuint generic_vs, generictex_vs, circle_vs, sphere_vs;
	GLuint generic_fs, generictex_fs, generictext_fs, ads_per_frag_fs;
	GLuint generic_sp, generictex_sp, generictext_sp, circle_sp, sphere_sp;
	Texture tex, fliessen, text, rock, charmap;

	mView = pIdentity();

	//Create Shader
	loadShader(&vsGenericStr, VS_GENERIC);
	loadShader(&fsGenericStr, FS_GENERIC);
	loadShader(&vsGenericTexStr, VS_GENERIC_TEX);
	loadShader(&fsGenericTexStr, FS_GENERIC_TEX);
	loadShader(&fsGenericTextStr, FS_GENERIC_TEXT);
	loadShader(&vsCircleStr, VS_CIRCLE);
	loadShader(&vsSphereStr, VS_SPHERE);
	loadShader(&fsADSperFragStr, FS_ADS_PER_FRAGMENT);

	generic_vs = createShader(GL_VERTEX_SHADER, vsGenericStr);
	generictex_vs = createShader(GL_VERTEX_SHADER, vsGenericTexStr);
	circle_vs = createShader(GL_VERTEX_SHADER, vsCircleStr);
	sphere_vs = createShader(GL_VERTEX_SHADER, vsSphereStr);

	generic_fs = createShader(GL_FRAGMENT_SHADER, fsGenericStr);
	generictex_fs = createShader(GL_FRAGMENT_SHADER, fsGenericTexStr);
	generictext_fs = createShader(GL_FRAGMENT_SHADER, fsGenericTextStr);
	ads_per_frag_fs = createShader(GL_FRAGMENT_SHADER, fsADSperFragStr);

	generic_sp = createShaderProgram(generic_vs, generic_fs);
	generictex_sp = createShaderProgram(generictex_vs, generictex_fs);
	generictext_sp = createShaderProgram(generictex_vs, generictext_fs);
	circle_sp = createShaderProgram(circle_vs, generic_fs);
	sphere_sp = createShaderProgram(sphere_vs, ads_per_frag_fs);

	free(vsGenericStr);
	free(fsGenericStr);
	free(vsGenericTexStr);
	free(fsGenericTexStr);
	free(fsGenericTextStr);
	free(vsCircleStr);
	free(vsSphereStr);
	free(fsADSperFragStr);

	//Texture
	initTexture(&tex, GL_TEXTURE0, GL_RGB, "res/green_scratches.bmp");
	initTexture(&fliessen, GL_TEXTURE0, GL_RGB, "res/fliessen.bmp");
	initTexture(&rock, GL_TEXTURE0, GL_RGB, "res/universum.bmp");
	initTexture(&text, GL_TEXTURE0, GL_RGB, "res/font3_new.bmp");
	initTexture(&charmap, GL_TEXTURE0, GL_RGB, "res/font.bin");
	printf("Texuture xSize = %u\n",tex.xSize);
	printf("Texuture ySize = %u\n",tex.ySize);
	printf("Texture ID = %d\n", tex.id);
	printf("Texture Index = %d\n", tex.textureIndex);

	//Text Block
	initObject(&txtBlock, pIdentity(), generictext_sp, "res/txtblock2.geo", FROM_FILE);
	txtBlock.renderMode = GL_TRIANGLE_STRIP;
	txtBlock.texID = text.id;
	txtBlock.isTex = GL_TRUE;
	//txtBlock.mModel = scaleMatrix(txtBlock.mModel,vec3(0.2,0.08,1.0));
	txtBlock.mModel = scaleMatrix(txtBlock.mModel,vec3(0.08,0.03,1.0));
	charOffset['1'] = 3*4*2*4;
	charOffset['2'] = 4*4*2*4;
	charOffset['3'] = 5*4*2*4;
	charOffset['4'] = 6*4*2*4;
	charOffset['5'] = 7*4*2*4;
	charOffset['6'] = 8*4*2*4;
	charOffset['7'] = 9*4*2*4;
	charOffset['8'] = 10*4*2*4;
	charOffset['9'] = 11*4*2*4;
	charOffset['0'] = 12*4*2*4;
	charOffset['.'] = 13*4*2*4;
	charOffset['-'] = 14*4*2*4;
	charOffset[' '] = 15*4*2*4;
	if (f1)
	{
		//Triangle
		initObject(&triangle, mView, generic_sp, "res/triangle.geo",FROM_FILE);
		triangle.renderMode = GL_TRIANGLES;
		triangle.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,1.0f));
		triangle.mModel = translateMatrix(triangle.mModel,vec3(-1.0f,0.5f,0.0f));
		triangle.color = getColor(0.0f, 1.0f, 1.0f, 1.0f);

		//Rectangle
		initObject(&rect, mView, generic_sp, "res/rectangle.geo", FROM_FILE);
		rect.renderMode = GL_TRIANGLE_STRIP;
		rect.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,1.0f));
		rect.mModel = translateMatrix(rect.mModel,vec3(1.0,-0.5,0.0));
		rect.color = getColor(1.0f,1.0f,0.0f,1.0f);
		rect.isTCO = GL_FALSE;

		//Cross
		initObject(&cross, mView, generic_sp, "res/cross.geo", FROM_FILE);
		cross.renderMode = GL_LINES;
		cross.mProj = identity();
		cross.mModel = identity();
		cross.color = getColor(1.0,0.0,0.0,1.0);

		//Circle
		initObject(&circle, mView, circle_sp, "res/circle.geo", FROM_FILE);
		circle.renderMode = GL_TRIANGLE_FAN;
		circle.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,1.0f));
		circle.mModel = translateMatrix(circle.mModel,vec3(1.0,0.5,0.0));
		circle.color = getColor(0.0,1.0,0.0,1.0);

		//Stern
		initObject(&stern, mView, generic_sp, "res/stern.geo", FROM_FILE);
		stern.renderMode = GL_TRIANGLES;
		stern.mModel = scaleMatrix(identity(),vec3(0.4f,0.4f,1.0f));
		stern.mModel = translateMatrix(stern.mModel,vec3(-1.0,-0.5,0.0));
		stern.color = getColor(0.8,0.0,0.8,1.0);

		//background
		initObject(&background, mView, generictex_sp, "res/rectangle.geo", FROM_FILE);
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
	}
	else if (f2)
	{
		//cube
		initObject(&cube, mView, generictex_sp, "res/cube.geo", FROM_FILE | OBJ_3D);
		cube.renderMode = GL_TRIANGLES;
		cube.mProj = getFrustum(0.25*(GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,0.25,0.5,50.0);
		//cube.mProj = getFrustum(0.25*(GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,0.25,0.5,25.0);
		cube.mModel = translateMatrix(identity(), vec3(0.0,0.0,-5.0));
		cube.texID = fliessen.id;
		cube.isTex = GL_TRUE;

		//cube2
		cube2 = cube;
		cube2.texID = rock.id;
		cube2.mProj = getFrustum(0.25*(GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,0.25,0.5,200000.0);
		cube2.mView = pIdentity();
		cube2.mModel = scaleMatrix(identity(),vec3(100000.0,100000.0,100000.0));

		//Sphere
		initObject(&sphere, mView, sphere_sp, NULL, MESH_GRID | OBJ_3D);
		sphere.renderMode = GL_TRIANGLES;
		sphere.mProj = getFrustum(0.25*(GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,0.25,0.5,50.0);
		//sphere.mModel = translateMatrix(identity(), vec3(0.0,0.0,-5.0));
		sphere.mTransInvModelView = identity();
		sphere.color = getColor(0.0,1.0,0.0,1.0);

		//Rectangle
		initObject(&plane, mView, generic_sp, "res/rectangle.geo", FROM_FILE);
		plane.renderMode = GL_TRIANGLE_STRIP;
		plane.mProj = getFrustum(0.25*(GLfloat)SCREEN_WIDTH/(GLfloat)SCREEN_HEIGHT,0.25,0.5,50.0);
		plane.color = getColor(1.0f,1.0f,0.0f,1.0f);
		plane.mModel = scaleMatrix(plane.mModel,vec3(10.0,10.0,1.0));
		plane.mModel = multMatrix(getRotX(3.14159/2.0),plane.mModel);
		plane.isTCO = GL_FALSE;
	}
}

void renderText(Object *o, char *text, GLfloat xPos, GLfloat yPos)
{
	uint i;
	//float bc[6]={0.3,0.5,0.3,0.5,0.3,0.5};

	o->mModel.m14 = -xPos;
	o->mModel.m24 = yPos;
	o->mModel.m34 = -1.0;

	for (i=0;i<strlen(text);i++)
	{
		o->vertexBufferOffset = i*4*3*4;
		o->texCoordBufferOffset = charOffset[(uint)text[i]];
		//o->color = getColor(bc[i],bc[i],bc[i],1.0);
		drawObject(o);
	}
}

void renderLoop()
{
	status ret;
	bool quit=FALSE;
	unsigned char keybuf[8];
	unsigned char gamectrlbuf[8];
	int transferred;
	//uint i,j;
	Matrix4 M, mModelView;
	int ipiv[4];
	uint frameCount=0;
	clock_t start_t, end_t, delta_t;
	char strFPS[4];
	char strPos[32];
	Vector3 rotAxis;

	memset(keybuf, 0, sizeof(keybuf));
	memset(gamectrlbuf, 128, sizeof(gamectrlbuf));

	sprintf(strFPS,"%u",0);
	start_t = clock(); //FPS
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
			renderText(&txtBlock,"1231",1.5,0.9);

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
		else if (f2)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//Matrizenberechnung
			mModelView = multPtrMatrix(mView,&sphere.mModel);
			setIdentity(&sphere.mTransInvModelView);
			LAPACKE_sgesv( LAPACK_ROW_MAJOR, 4, 4, (float*)&mModelView, 4, ipiv, (float*)&sphere.mTransInvModelView, 4 );

			//drawObject(&cube);
			drawObject(&sphere);
			drawObject(&cube2);
			drawObject(&plane);
/*
			for (i=0;i<10;i++)
			{
				for (j=0;j<10;j++)
				{
					setPtrPosition(&cube.mModel,pTmpVec3((float)i*5.0+5.0,0.0,-(float)j*5.0));
					drawObject(&cube);
				}
			}
*/
			sprintf(strPos,"%1.1f",-mView->m14*mView->m31 - mView->m24*mView->m32 + mView->m34*mView->m33);
			renderText(&txtBlock,strPos,1.5,0.8);
			renderText(&txtBlock,strFPS,1.5,0.9);

			eglSwapBuffers(state->display, state->surface);

			frameCount++;
			end_t = clock();
			delta_t = (double)(end_t - start_t) / CLOCKS_PER_SEC;
			if (delta_t>1.0)
			{
				sprintf(strFPS,"%u",frameCount);
				start_t = clock();
				frameCount = 0;
			}

			ret = libusb_bulk_transfer(usb_dev->hid_keyboard,ENDPOINT_ADDRESS,keybuf,5,&transferred,1);
			if (ret==0 || ret==-7)
			{
				if (keybuf[2]==0x1a || keybuf[3]==0x1a) translatePtrMatrix(mView,pTmpVec3(0.0,0.0,0.05));
				if (keybuf[2]==0x16 || keybuf[3]==0x16) translatePtrMatrix(mView,pTmpVec3(0.0,0.0,-0.05));
				if (keybuf[2]==0x04 || keybuf[3]==0x04) translatePtrMatrix(mView,pTmpVec3(0.05,0.0,0.0));
				if (keybuf[2]==0x07 || keybuf[3]==0x07) translatePtrMatrix(mView,pTmpVec3(-0.05,0.0,0.0));
				if (keybuf[2]==0x1b || keybuf[3]==0x1b) translatePtrMatrix(mView,pTmpVec3(0.0,0.05,0.0));
				if (keybuf[2]==0x1d || keybuf[3]==0x1d) translatePtrMatrix(mView,pTmpVec3(0.0,-0.05,0.0));
				if (keybuf[2]==0x4f || keybuf[3]==0x4f) *mView = multPtrMatrix(pTmpGetRotY(0.05),mView);
				if (keybuf[2]==0x50 || keybuf[3]==0x50) *mView = multPtrMatrix(pTmpGetRotY(-0.05),mView);
				if (keybuf[2]==0x51 || keybuf[3]==0x51) *mView = multPtrMatrix(pTmpGetRotX(0.05),mView);
				if (keybuf[2]==0x52 || keybuf[3]==0x52) *mView = multPtrMatrix(pTmpGetRotX(-0.05),mView);
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
				translatePtrMatrix(mView, pTmpVec3(0.0, 0.0, -((GLfloat)gamectrlbuf[1]/256.0-0.5)));
				translatePtrMatrix(mView, pTmpVec3(-((GLfloat)gamectrlbuf[0]/256.0-0.5), 0.0, 0.0));
				*mView = multPtrMatrix(pTmpGetRotX(-((GLfloat)gamectrlbuf[3]/256.0-0.5)), mView);

				*cube2.mView = multPtrMatrix(pTmpGetRotY((GLfloat)gamectrlbuf[2]/256.0-0.5), cube2.mView);

                rotAxis.x = mView->m12;
                rotAxis.y = mView->m22;
                rotAxis.z = mView->m32;
				*mView = multPtrMatrix(pTmpGetRotX(((GLfloat)gamectrlbuf[2]/256.0-0.5)*rotAxis.x), mView);
				//CBLAS-Test
				M=*mView;
				cblas_sgemm(CblasRowMajor,CblasNoTrans,CblasNoTrans,4,4,4,1.0,(float*)pTmpGetRotY(((GLfloat)gamectrlbuf[2]/256.0-0.5)*rotAxis.y),4,(float*)&M,4,0.0,(float*)mView,4);
				*mView = multPtrMatrix(pTmpGetRotZ(((GLfloat)gamectrlbuf[2]/256.0-0.5)*rotAxis.z), mView);
			}
			else
			{
				printf("Transfer Error: %d\n",ret);
				quit=TRUE;
			}
		}
	}
}

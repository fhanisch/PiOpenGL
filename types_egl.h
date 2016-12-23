#ifndef TYPES_EGL_H
#define TYPES_EGL_H

typedef struct
{
	uint32_t screen_width;
	uint32_t screen_height;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
} CUBE_STATE_T;

#endif

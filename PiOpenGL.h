#ifndef PIOPENGL_H
#define PIOPENGL_H

typedef enum {ok, err} status;
typedef char bool;

typedef struct
{
	uint32_t screen_width;
	uint32_t screen_height;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
} CUBE_STATE_T;

#define TRUE 1
#define FALSE 0

#define ENDPOINT_ADDRESS 0x81

#endif


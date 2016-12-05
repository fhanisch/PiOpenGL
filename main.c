/*
 * =====================================================================
 * 
 * PiOpenGL
 * 
 * 05.12.2016
 * 
 * =====================================================================
 */
 
#include <stdio.h>
#include <bcm_host.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL.h>

#define WINDOW_NAME "PiOpenGL"

typedef enum {ok, err} status;

typedef struct
{
	DISPMANX_ELEMENT_HANDLE_T element;
	int width;
	int height;
} EGL_DISPMANX_WINDOW_T;

typedef struct
{
	uint32_t screen_width;
	uint32_t screen_height;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
} CUBE_STATE_T;

status createOpenGLContext(CUBE_STATE_T *p_state)
{
	int32_t st;
	EGLBoolean result;
	EGLint num_config;
	EGLConfig config;
	static EGL_DISPMANX_WINDOW_T nativewindow;

	DISPMANX_ELEMENT_HANDLE_T dispman_element;
	DISPMANX_DISPLAY_HANDLE_T dispman_display;
	DISPMANX_UPDATE_HANDLE_T dispman_update;
	VC_RECT_T dst_rect;
	VC_RECT_T src_rect;

	static const EGLint attribute_list[] =
	{
	    EGL_RED_SIZE, 8,
	    EGL_GREEN_SIZE, 8,
	    EGL_BLUE_SIZE, 8,
	    EGL_ALPHA_SIZE, 8,
	    EGL_DEPTH_SIZE, 16,
	    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
	    EGL_NONE
	};

    static const EGLint context_attributes[] =
	{
	    EGL_CONTEXT_CLIENT_VERSION, 2,
	    EGL_NONE
	};

	bcm_host_init();
	p_state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);	
	result = eglInitialize(p_state->display, NULL, NULL);
	result = eglChooseConfig(p_state->display, attribute_list, &config, 1, &num_config);		
	result = eglBindAPI(EGL_OPENGL_ES_API);	
	p_state->context = eglCreateContext(p_state->display, config, EGL_NO_CONTEXT,context_attributes);	
	st = graphics_get_display_size(0, &p_state->screen_width,&p_state->screen_height);
	if (st) return err;
	p_state->screen_width = 1920;
	p_state->screen_height = 1080;
	dst_rect.x = 0;
	dst_rect.y = 0;
	dst_rect.width = p_state->screen_width;
	dst_rect.height = p_state->screen_height;
	src_rect.x = 0;
	src_rect.y = 0;
	src_rect.width = p_state->screen_width << 16;
	src_rect.height = p_state->screen_height << 16;        
	dispman_display = vc_dispmanx_display_open(0);
	dispman_update = vc_dispmanx_update_start(0);
	dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display,0, &dst_rect, 0,&src_rect, DISPMANX_PROTECTION_NONE, 0, 0, 0);
	nativewindow.element = dispman_element;
	nativewindow.width = p_state->screen_width;
	nativewindow.height = p_state->screen_height;
	vc_dispmanx_update_submit_sync( dispman_update );
	p_state->surface = eglCreateWindowSurface( p_state->display, config, (EGLNativeWindowType)&nativewindow, NULL );		
	result = eglMakeCurrent(p_state->display, p_state->surface, p_state->surface, p_state->context);
	if (!result) return err;
	
	return ok;
}

status createOpenGLWindow(char *windowName,SDL_Window *window)
{
	window = SDL_CreateWindow(windowName,50,50,400,400,SDL_WINDOW_OPENGL);
	if (window==NULL) return err;
	return ok;
}
 
int main(int argc, char *argv[])
{
	CUBE_STATE_T state;
	//SDL_Window *window=NULL;
	//SDL_GLContext glcontext;
	status st;
	
	printf("PiOPenGL\n");
	printf("========\n\n");
	
	st=createOpenGLContext(&state);
	if (st)
	{
		printf("OpenGLContext konnte nicht erstellt werden!\n");
		return err;
	}
	printf("OpenGLContext erstellt!\n");
	/*
	st=createOpenGLWindow(WINDOW_NAME,window); // SDL Window nicht notwendig --> OpenGLContext funktioniert auch so, da oben bereits erstellt. Nur für Events nötig.
	if (st)
	{
		printf("OpenGLWindow konnte nicht erstellt werden!\n");
		return err;
	}
	printf("OpenGLWindow erstellt!\n");
	
	glcontext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window,glcontext);
	*/
	glClearColor(1.0f,0.0f,1.0f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//SDL_GL_SwapWindow(window);
	eglSwapBuffers(state.display, state.surface); //für Fullscreen	
	
	//SDL_GL_DeleteContext(glcontext);
	//SDL_DestroyWindow(window);
	SDL_Delay(5000);
	//SDL_Quit();
	
	return 0;
}

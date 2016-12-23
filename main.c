/**
    =====================================================================

    PiOpenGL

    @date 05.12.2016

    @author FH

    =====================================================================
*/

#include <stdio.h>
#include <libusb-1.0/libusb.h>
#include <bcm_host.h>
#include <EGL/egl.h>
#include "types_egl.h"
#include "PiOpenGL.h"
#include "renderer.h"

// Logitech - USB Receiver
#define VENDOR_ID 0x046d
#define PRODUCT_ID 0xc517
#define KEYBOARD_INTERFACE 0

typedef struct
{
	DISPMANX_ELEMENT_HANDLE_T element;
	int width;
	int height;
} EGL_DISPMANX_WINDOW_T;

status createOpenGLContext(CUBE_STATE_T *p_state)
{
	int32_t ret;
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
	ret = graphics_get_display_size(0, &p_state->screen_width,&p_state->screen_height);
	if (ret) return err;
	p_state->screen_width = SCREEN_WIDTH;
	p_state->screen_height = SCREEN_HEIGHT;
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

status init_keyboard(libusb_device_handle **usb_dev)
{
	status ret;

	ret = libusb_init(NULL);
	if (ret)
	{
		printf("USB konnte nicht initialisiert werden!\n");
		return err;
	}
	printf("USB initialisiert!\n");

	*usb_dev = libusb_open_device_with_vid_pid(NULL,VENDOR_ID,PRODUCT_ID);
	if (!*usb_dev)
	{
		printf("USB Device wurde nicht gefunden!\n");
		return err;
	}
	printf("USB Device erkannt!\n");

	libusb_detach_kernel_driver(*usb_dev,KEYBOARD_INTERFACE);

	ret = libusb_claim_interface(*usb_dev,KEYBOARD_INTERFACE);
	if (ret < 0)
	{
		printf("Claim fehlgeschlagen: %d!\n", ret);
		return 3;
	}
	printf("USB interfaces claimed!\n");

	return ok;
}

void close_keyboard(libusb_device_handle **usb_dev)
{
	libusb_release_interface(*usb_dev, KEYBOARD_INTERFACE);
	libusb_attach_kernel_driver(*usb_dev,KEYBOARD_INTERFACE);
	libusb_close(*usb_dev);
	libusb_exit(NULL);
}

int main(int argc, char *argv[])
{
	status ret;
	CUBE_STATE_T state;
	libusb_device_handle *usb_dev = NULL;

	printf("*** PiOpenGL ***\n");
	printf("================\n\n");

	ret = init_keyboard(&usb_dev);
	if (ret) return err;

	ret=createOpenGLContext(&state);
	if (ret)
	{
		printf("OpenGLContext konnte nicht erstellt werden!\n");
		return err;
	}
	printf("OpenGLContext erstellt!\n");

	initOpenGL(&state, usb_dev);
	initRenderScene();
	renderLoop();

	close_keyboard(&usb_dev);
	return 0;
}

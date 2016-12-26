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
#include <SDL/SDL_ttf.h>
#include "types.h"
#include "PiOpenGL.h"
#include "renderer.h"

// Logitech - USB Receiver
#define KEYBOARD_VENDOR_ID 0x046d
#define KEYBOARD_PRODUCT_ID 0xc517
// Logitech - USB Game Controller
#define GAMECONTROLLER_VENDOR_ID 0x046d
#define GAMECONTROLLER_PRODUCT_ID 0xc218

#define INTERFACE 0

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

status init_hid(libusb_device_handle **hid_dev, uint vendor_id, uint product_id, char *deviceName)
{
	status ret;

	ret = libusb_init(NULL);
	if (ret)
	{
		printf("USB konnte nicht initialisiert werden!\n");
		return err;
	}
	printf("USB initialisiert!\n");

	*hid_dev = libusb_open_device_with_vid_pid(NULL,vendor_id,product_id);
	if (!*hid_dev)
	{
		printf("USB %s wurde nicht gefunden!\n",deviceName);
		return err;
	}
	printf("USB %s erkannt!\n",deviceName);

	libusb_detach_kernel_driver(*hid_dev,INTERFACE);

	ret = libusb_claim_interface(*hid_dev,INTERFACE);
	if (ret < 0)
	{
		printf("Claim %s Interface fehlgeschlagen: %d!\n", deviceName, ret);
		return 3;
	}
	printf("USB %s interface claimed!\n",deviceName);

	return ok;
}

void close_hid(libusb_device_handle *hid_dev)
{
	libusb_release_interface(hid_dev, INTERFACE);
	libusb_attach_kernel_driver(hid_dev,INTERFACE);
	libusb_close(hid_dev);
	libusb_exit(NULL);
}

int main(int argc, char *argv[])
{
	status ret;
	CUBE_STATE_T state;
	USB_DEV usb_dev;

	printf("*** PiOpenGL ***\n");
	printf("================\n\n");

	ret = init_hid(&usb_dev.hid_keyboard,KEYBOARD_VENDOR_ID, KEYBOARD_PRODUCT_ID, "Keyboard");
	if (ret) return err;
	ret = init_hid(&usb_dev.hid_gamecontroller,GAMECONTROLLER_VENDOR_ID, GAMECONTROLLER_PRODUCT_ID, "Game Controller");
	if (ret) return err;

	ret=createOpenGLContext(&state);
	if (ret)
	{
		printf("OpenGLContext konnte nicht erstellt werden!\n");
		return err;
	}
	printf("OpenGLContext erstellt!\n");

	TTF_Init(); //init SDL True Type Fonts

	initOpenGL(&state, &usb_dev);
	initRenderScene();
	renderLoop();

	close_hid(usb_dev.hid_keyboard);
	close_hid(usb_dev.hid_gamecontroller);
	return 0;
}

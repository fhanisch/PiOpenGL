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

typedef struct
{
	libusb_device_handle *hid_keyboard;
	libusb_device_handle *hid_mouse;
	libusb_device_handle *hid_gamecontroller;
} USB_DEV;

#endif

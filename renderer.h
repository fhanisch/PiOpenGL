#ifndef RENDERER_H
#define RENDERER_H

void initOpenGL(CUBE_STATE_T *s, libusb_device_handle *dev);
void initRenderScene();
void renderLoop();

#endif

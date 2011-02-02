
/**
 * libemulation-hal
 * OpenGL canvas
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenGL canvas.
 */

#ifndef _OPENGLHAL_H
#define _OPENGLHAL_H

#include <pthread.h>

#include <OpenGL/gl.h>

#include "OEComponent.h"
#include "CanvasInterface.h"

typedef enum
{
	OPENGLHAL_TEXTURE_POWER,
	OPENGLHAL_TEXTURE_PAUSE,
	OPENGLHAL_TEXTURE_CAPTURE,
	OPENGLHAL_TEXTURE_FRAME,
	OPENGLHAL_TEXTURE_INTERLACE,
	OPENGLHAL_TEXTURE_END,
} OEOpenGLTextureIndex;

typedef enum
{
	OPENGLHAL_CAPTURE_NONE,
	OPENGLHAL_CAPTURE_KEYBOARD_AND_DISCONNECT_MOUSE_CURSOR,
	OPENGLHAL_CAPTURE_KEYBOARD_AND_HIDE_MOUSE_CURSOR,
} OpenGLHALCapture;

typedef enum
{
	OPENGLHAL_PROGRAM_RGB,
	OPENGLHAL_PROGRAM_COMPOSITE,
	OPENGLHAL_PROGRAM_END,
} OpenGLHALProgram;



typedef void (*CanvasSetCapture)(void *userData, OpenGLHALCapture capture);
typedef void (*CanvasSetKeyboardFlags)(void *userData, int flags);



class OpenGLHAL : public OEComponent
{
public:
	OpenGLHAL();
	
	void open(CanvasSetCapture setCapture,
			  CanvasSetKeyboardFlags setKeyboardFlags,
			  void *userData);
	void close();
	
	void setShader(bool value);
	
	OESize getDefaultViewSize();
	bool update(float width, float height, float offset, bool update);
	
	void becomeKeyWindow();
	void resignKeyWindow();
	
	void sendSystemEvent(int usageId);
	void setKey(int usageId, bool value);
	void sendUnicodeKeyEvent(int unicode);
	
	void setMouseButton(int index, bool value);
	void enterMouse();
	void exitMouse();
	void setMousePosition(float x, float y);
	void moveMouse(float rx, float ry);
	void sendMouseWheelEvent(int index, float value);
	
	void setJoystickButton(int deviceIndex, int index, bool value);
	void setJoystickPosition(int deviceIndex, int index, float value);
	void sendJoystickHatEvent(int deviceIndex, int index, float value);
	void moveJoystickBall(int deviceIndex, int index, float value);
	
	bool copy(string& value);
	bool paste(string value);
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	CanvasSetCapture setCapture;
	CanvasSetKeyboardFlags setKeyboardFlags;
	void *userData;
	
	pthread_mutex_t drawMutex;
	
	bool enableShader;
	
	CanvasConfiguration configuration;
	bool updateConfiguration;
	
	CanvasConfiguration frameConfiguration;
	OEImage *nextFrame;
	OEImage *frame;
	
	GLuint glTextures[OPENGLHAL_TEXTURE_END];
	OESize glTextureSize;
	GLuint glPrograms[OPENGLHAL_PROGRAM_END];
	GLuint glProgram;
	
	OpenGLHALCapture capture;
	
	bool keyDown[CANVAS_KEYBOARD_KEY_NUM];
	int keyDownCount;
	bool ctrlAltWasPressed;
	bool mouseEntered;
	bool mouseButtonDown[CANVAS_MOUSE_BUTTON_NUM];
	bool joystickButtonDown[CANVAS_JOYSTICK_NUM][CANVAS_JOYSTICK_BUTTON_NUM];
	
	bool initOpenGL();
	void freeOpenGL();
	bool loadShaders();
	GLuint loadShader(const char *source);
	bool updateShader();
	bool updateTexture();
	bool drawCanvas(float width, float height);
	
	void postHIDNotification(int notification, int usageId, float value);
	void updateCapture(OpenGLHALCapture capture);
	void resetKeysAndButtons();
	
	bool setCaptureMode(CanvasCaptureMode *captureMode);
	bool setConfiguration(CanvasConfiguration *configuration);
	bool postFrame(OEImage *frame);
};

#endif

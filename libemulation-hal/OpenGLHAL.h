
/**
 * OpenEmulator
 * OpenGL canvas
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
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
	OEGL_TEX_POWER,
	OEGL_TEX_PAUSE,
	OEGL_TEX_CAPTURE,
	OEGL_TEX_FRAME,
	OEGL_TEX_INTERLACE,
	OEGL_TEX_NUM,
} OEOpenGLTextureIndex;

class OpenGLHAL : public OEComponent
{
public:
	OpenGLHAL();
	~OpenGLHAL();
	
	void initOpenGL();
	void draw(int width, int height);
	string getDefaultCanvasSize();
	
	void setSystemKey(int usageId);
	void setKey(int usageId, bool value);
	void setUnicodeKey(int unicode);
	
	void setMouseButton(int index, bool value);
	void setMousePosition(float x, float y);
	void moveMouse(float rx, float ry);
	void sendMouseWheelEvent(int index, float value);
	
	void setJoystickButton(int deviceIndex, int index, bool value);
	void setJoystickPosition(int deviceIndex, int index, float value);
	void sendJoystickHatEvent(int deviceIndex, int index, float value);
	void moveJoystickBall(int deviceIndex, int index, float value);
	
	void resetKeysAndButtons();
	
private:
	GLuint textures[OEGL_TEX_NUM];
	
	pthread_mutex_t frameMutex;
	
	bool keyDown[CANVAS_KEYBOARD_KEY_NUM];
	int keyDownCount;
	bool mouseButtonDown[CANVAS_MOUSE_BUTTON_NUM];
	bool joystickButtonDown[CANVAS_JOYSTICK_NUM][CANVAS_JOYSTICK_BUTTON_NUM];
	
	bool mouseCaptured;
	bool mouseCaptureRelease;
	
	void postHIDNotification(int notification, int usageId, float value);
};

#endif

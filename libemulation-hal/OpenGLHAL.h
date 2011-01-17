
/**
 * OpenEmulator
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
	OEGL_TEX_POWER,
	OEGL_TEX_PAUSE,
	OEGL_TEX_CAPTURE,
	OEGL_TEX_FRAME,
	OEGL_TEX_INTERLACE,
	OEGL_TEX_NUM,
} OEOpenGLTextureIndex;



typedef void (*CanvasSetCapture)(void *userData, CanvasCapture capture);
typedef void (*CanvasSetKeyboardFlags)(void *userData, int flags);



class OpenGLHAL : public OEComponent
{
public:
	OpenGLHAL();
	
	void open(CanvasSetCapture setCapture,
			  CanvasSetKeyboardFlags setKeyboardFlags,
			  void *userData);
	void close();
	
	OESize getDefaultSize();
	void draw(int width, int height);
	
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
	
	bool copy(string &value);
	bool paste(string value);
	
	bool postMessage(OEComponent *sender, int message, void *data);
	
private:
	CanvasSetCapture setCapture;
	CanvasSetKeyboardFlags setKeyboardFlags;
	void *userData;
	
	CanvasCaptureMode captureMode;
	CanvasCapture capture;
	
	GLuint textures[OEGL_TEX_NUM];
	
	bool keyDown[CANVAS_KEYBOARD_KEY_NUM];
	int keyDownCount;
	bool ctrlAltWasPressed;
	bool mouseEntered;
	bool mouseButtonDown[CANVAS_MOUSE_BUTTON_NUM];
	bool joystickButtonDown[CANVAS_JOYSTICK_NUM][CANVAS_JOYSTICK_BUTTON_NUM];
	
	OESize defaultSize;
	
	void postHIDNotification(int notification, int usageId, float value);
	void updateCapture(CanvasCapture capture);
	void releaseKeysAndButtons();
	
	bool getFrame(CanvasFrame *frame);
	bool returnFrame(CanvasFrame *frame);
};

#endif

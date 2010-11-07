
/**
 * OpenEmulator
 * OpenEmulator HID interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator HID interface.
 */

#ifndef _OEHID_H
#define _OEHID_H

#include "OEPAEmulation.h"
#include "HostInterface.h"


#include <pthread.h>

enum 
{
	OEGL_TEX_FRAME,
	OEGL_TEX_BEZELPOWER,
	OEGL_TEX_BEZELPAUSE,
	OEGL_TEX_BEZELCAPTURE,
	OEGL_TEX_NUM,
};

typedef struct
{
	float x;
	float y;
} OEPoint;

typedef struct
{
	float width;
	float height;
} OESize;

typedef struct
{
	OEPoint origin;
	OESize size;
} OERect;

class OEGL
{
public:
	OEGL();
	~OEGL();
	
	//	void update(HostVideoScreens *screens);
	void draw(int width, int height);
	
private:
	GLuint texture[OEGL_TEX_NUM];
	pthread_mutex_t glMutex;
	
	OESize windowSize;
	float windowAspectRatio;
	bool windowRedraw;
	
	//	HostVideoScreens screens;
	
	int columnNum;
	int rowNum;
	OESize cellSize;
	
	//	OESize getScreenSize(HostVideoConfiguration *conf);
	
	void updateCellSize();
	void updateScreenMatrix();
	
	//	void drawScreen(HostVideoScreen *screen, int index);
	
	//	void renderScreen(OEUInt32 *framebuffer, OESize framebufferSize,
	//					  OERect frame);
};

#define OEHID_KEY_NUM				256
#define OEHID_MOUSEBUTTON_NUM		8
#define OEHID_JOYSTICK_NUM			4
#define OEHID_JOYSTICKBUTTON_NUM	8

typedef void (*OEHIDCallback)(void *userData, int value);

class OEHID
{
public:
	OEHID(OEPAEmulation *emulation,
		  OEHIDCallback setMouseCapture,
		  OEHIDCallback setKeyboardLEDs);
	
	void sendSystemEvent(int usageId);
	void setKey(int usageId, bool value);
	void sendUnicode(int unicode);
	
	void setMouseButton(int index, bool value);
	void setMousePosition(float x, float y);
	void moveMouse(float rx, float ry);
	void sendMouseWheelEvent(int index, float value);
	
	void setJoystickButton(int deviceIndex, int index, bool value);
	void setJoystickPosition(int deviceIndex, int index, float value);
	void sendJoystickHatEvent(int deviceIndex, int index, float value);
	void moveJoystickBall(int deviceIndex, int index, float value);
	
	void reset();
	
private:
	OEPAEmulation *emulation;
	
	OEHIDCallback setMouseCapture;
	OEHIDCallback setKeyboardLEDs;
	
	int keyDownCount;
	bool keyDown[OEHID_KEY_NUM];
	bool mouseButtonDown[OEHID_MOUSEBUTTON_NUM];
	bool joystickButtonDown[OEHID_JOYSTICK_NUM][OEHID_JOYSTICKBUTTON_NUM];
	
	bool mouseCapture;
	bool mouseCaptured;
	bool mouseCaptureRelease;
	
	void sendHIDEvent(int notification, int usageId, float value);
};

#endif

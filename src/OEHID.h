
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
#include "Host.h"

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
	
	void setTabletButton(int index, bool value);
	void setTabletPosition(float x, float y);
	void setTabletProximity(bool value);
	
private:
	OEPAEmulation *emulation;
	
	OEHIDCallback setMouseCapture;
	OEHIDCallback setKeyboardLEDs;
	
	int keyDownCount;
	bool keyDown[HOST_HID_KEY_NUM];
	bool mouseButtonDown[HOST_HID_MOUSE_BUTTON_NUM];
	bool joystickButtonDown[HOST_HID_JOYSTICK_NUM][HOST_HID_JOYSTICK_BUTTON_NUM];
	bool tabletButtonDown[HOST_HID_TABLET_BUTTON_NUM];
	
	bool mouseCaptured;
	bool mouseCaptureRelease;
	
	void sendHIDEvent(int notification, int usageId, float value);
};

#endif

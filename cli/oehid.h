
/**
 * OpenEmulator
 * OpenEmulator/HID interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/HID interface.
 */

#include "OEEmulation.h"

#include "oepa.h"

#define OEHID_KEYDOWN_SIZE		256
#define OEHID_MOUSEDOWN_SIZE	8
#define OEHID_JOYSTICK_NUM		4
#define OEHID_JOYSTICKDOWN_SIZE	16
#define OEHID_TABLETDOWN_SIZE	8

typedef void (*OEHIDSetCaptureCallback)(void *userData, bool value);

class OEHID
{
public:
	OEHID(OEPAEmulation *emulation,
		  OEHIDSetCaptureCallback setCaptureCallback);
	~OEHID();
	
	void sendSystemEvent(int usageId);
	void setKey(int usageId, bool value);
	void sendUnicode(int unicode);
	
	void setMouseButton(int index, bool value);
	void setMousePosition(float x, float y);
	void moveMouse(float rx, float ry);
	void sendMouseWheelEvent(int index, float value);
	
	void setJoystickButton(int deviceIndex, int index, bool value);
	void setJoystickPosition(int deviceIndex, float x, float y);
	void sendJoystickHatEvent(int deviceIndex, int index, float value);
	void moveJoystickBall(int deviceIndex, int index, float value);
	
	void oehidSetTabletButton(bool value);
	void oehidSetTabletPosition(float x, float y);
	void oehidSetTabletProximity(bool value);
	
private:
	OEPAEmulation *emulation;
	OEHIDSetCaptureCallback setCaptureCallback;
	
	bool keyDown[OEHID_KEYDOWN_SIZE];
	bool mouseDown[OEHID_MOUSEDOWN_SIZE];
	bool joystickDown[OEHID_JOYSTICK_NUM][OEHID_JOYSTICKDOWN_SIZE];
	bool tabletDown[OEHID_TABLETDOWN_SIZE];
	
	bool mouseCaptured;
	bool mouseCaptureRelease;
	
	void send(int notification, int usageId, float value);
};

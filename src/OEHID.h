
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

#define OEHID_KEY_NUM				256
#define OEHID_MOUSEBUTTON_NUM		8
#define OEHID_JOYSTICKDEVICE_NUM	4
#define OEHID_JOYSTICKBUTTON_NUM	16
#define OEHID_JOYSTICKAXIS_NUM		16
#define OEHID_TABLETBUTTON_NUM		8

typedef void (*OEHIDCallback)(void *userData, int value);

class OEHID
{
public:
	OEHID(OEPAEmulation *emulation,
		  OEHIDCallback setMouseCapture,
		  OEHIDCallback setKeyboardLEDs);
	~OEHID();
	
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
	
	void setTabletButton(bool value);
	void setTabletPosition(float x, float y);
	void setTabletProximity(bool value);
	
private:
	OEPAEmulation *emulation;
	OEHIDCallback setMouseCapture;
	OEHIDCallback setKeyboardLEDs;
	
	int keyDownCount;
	bool keyDown[OEHID_KEY_NUM];
	bool mouseButtonDown[OEHID_MOUSEBUTTON_NUM];
	bool joystickButtonDown[OEHID_JOYSTICKDEVICE_NUM][OEHID_JOYSTICKBUTTON_NUM];
	bool tabletButtonDown[OEHID_TABLETBUTTON_NUM];
	
	bool mouseCaptured;
	bool mouseCaptureRelease;
	
	void send(int notification, int usageId, float value);
};


/**
 * OpenEmulator
 * OpenEmulator/HID interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/HID interface.
 */

#include "OEEmulation.h"

#define OEHID_KEYDOWN_SIZE		256
#define OEHID_MOUSEDOWN_SIZE	8
#define OEHID_JOYSTICK_NUM		4
#define OEHID_JOYSTICKDOWN_SIZE	16
#define OEHID_TABLETDOWN_SIZE	8

typedef void (*HIDSetCaptureCallback)(void *userData, bool value);

typedef struct
{
	OEEmulation *emulation;
	HIDSetCaptureCallback setCaptureCallback;
	
	bool keyDown[OEHID_KEYDOWN_SIZE];
	bool mouseDown[OEHID_MOUSEDOWN_SIZE];
	bool joystickDown[OEHID_JOYSTICK_NUM][OEHID_JOYSTICKDOWN_SIZE];
	bool tabletDown[OEHID_TABLETDOWN_SIZE];
	
	BOOL mouseButtonState[DOCUMENT_MOUSE_BUTTONNUM];
	BOOL mouseCaptured;
	BOOL mouseCaptureRelease;
} OEHIDContext;

OEHIDContext *oehidOpen(OEEmulation *emulation,
						HIDSetCaptureCallback setCaptureCallback);
void oehidClose(OEHIDContext *context);

void oehidSendSystemEvent(OEHIDContext *context, int usageId);

void oehidSetKey(OEHIDContext *context, int usageId, bool value);
void oehidSendUnicode(OEHIDContext *context, int unicode);

void oehidSetMouseButton(OEHIDContext *context, int index, bool value);
void oehidSetMousePosition(OEHIDContext *context, float x, float y);
void oehidMoveMouse(OEHIDContext *context, float rx, float ry);
void oehidSendMouseWheelEvent(OEHIDContext *context, int index, float value);

void oehidSetJoystickButton(OEHIDContext *context,
							int deviceIndex, int index, bool value);
void oehidSetJoystickPosition(OEHIDContext *context,
							  int deviceIndex, float x, float y);
void oehidSendJoystickHatEvent(OEHIDContext *context,
							   int deviceIndex, int index, float value);
void oehidMoveJoystickBall(OEHIDContext *context,
						   int deviceIndex, int index, float value);

void oehidSetTabletButton(OEHIDContext *context, bool value);
void oehidSetTabletPosition(OEHIDContext *context, float x, float y);
void oehidSetTabletProximity(OEHIDContext *context, bool value);

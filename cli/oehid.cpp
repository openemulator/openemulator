
/**
 * OpenEmulator
 * OpenEmulator/HID interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/HID interface.
 */

#include "oehid.h"
#include "oepa.h"

#include "Host.h"

OEHID::OEHID(OEPAEmulation *emulation,
			 OEHIDSetCaptureCallback setCaptureCallback)
{
	this->emulation = emulation;
	this->setCaptureCallback = setCaptureCallback;
	
	memset(keyDown, sizeof(keyDown), 0);
	memset(mouseDown, sizeof(mouseDown), 0);
}

OEHID::~OEHID()
{
}

void OEHID::send(int notification, int usageId, float value)
{
	HostHIDEvent hidEvent;
	hidEvent.usageId = usageId;
	hidEvent.value = value;
	
	emulation->postNotification(HOST_DEVICE, notification, &hidEvent);

	printf("%d %d %f\n", notification, usageId, value);
}

void OEHID::sendSystemEvent(int usageId)
{
	send(HOST_HID_SYSTEM_EVENT, usageId, 0);
}

void OEHID::setKey(int usageId, bool value)
{
	
}

void OEHID::sendUnicode(int unicode)
{
	if (unicode == 127)
		unicode = 8;
	
	if ((unicode < 0xf700) || (unicode >= 0xf900))
		send(HOST_HID_UNICODEKEYBOARD_EVENT, unicode, 0);
}

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

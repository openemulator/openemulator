
/**
 * OpenEmulator
 * OpenEmulator/HID interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator/HID interface.
 */

#include "oehid.h"

OEHIDContext *oehidOpen(OEEmulation *emulation,
						HIDSetCaptureCallback setCaptureCallback)
{
	OEHIDContext *context = new OEHIDContext;
	
	context->emulation = emulation;
	context->setCaptureCallback = setCaptureCallback;
	
	memset(context->keyDown, sizeof(context->keyDown), 0);
	memset(context->mouseDown, sizeof(context->mouseDown), 0);
	
	return context;
}

void oehidClose(OEHIDContext *context)
{
	delete context;
}

void oehidSend(OEHIDContext *context, int usageId, float value)
{
		context->emulation->postNotificaiton
}

void oehidSendSystemEvent(OEHIDContext *context, int usageId)
{
	
}

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

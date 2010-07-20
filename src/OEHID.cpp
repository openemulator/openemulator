
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
			 OEHIDCallback setMouseCapture,
			 OEHIDCallback setKeyboardLEDs)
{
	this->emulation = emulation;
	this->setMouseCapture = setMouseCapture;
	this->setKeyboardLEDs = setKeyboardLEDs;
	
	keyDownCount = 0;
	memset(keyDown, sizeof(keyDown), 0);
	memset(mouseButtonDown, sizeof(mouseButtonDown), 0);
	memset(joystickButtonDown, sizeof(joystickButtonDown), 0);
	memset(tabletButtonDown, sizeof(tabletButtonDown), 0);
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
	if (keyDown[usageId] == value)
		return;
	
	keyDown[usageId] = value;
	
	send(HOST_HID_KEYBOARD_EVENT, usageId, value);
	
	int count = keyDownCount + (value ? 1 : -1);
	keyDownCount = count < 0 ? 0 : count;
	
	if ((keyDown[HOST_HID_K_LEFTCONTROL] ||
		 keyDown[HOST_HID_K_RIGHTCONTROL]) &&
		(keyDown[HOST_HID_K_LEFTALT] ||
		 keyDown[HOST_HID_K_RIGHTALT]))
		mouseCaptureRelease = true;
	
	if (mouseCaptureRelease && !keyDownCount)
	{
		mouseCaptureRelease = false;
		mouseCaptured = false;
		setMouseCapture(emulation, false);
	}
}

void OEHID::sendUnicode(int unicode)
{
	if (unicode == 127)
		unicode = 8;
	
	if ((unicode < 0xf700) || (unicode >= 0xf900))
		send(HOST_HID_UNICODEKEYBOARD_EVENT, unicode, 0);
}

void OEHID::setMouseButton(int index, bool value)
{
	if (index >= OEHID_MOUSEBUTTON_NUM)
		return;
	
	if (mouseButtonDown[index] == value)
		return;
	
	mouseButtonDown[index] = value;
	
	if (mouseCaptured)
		send(HOST_HID_MOUSE_EVENT,
			 HOST_HID_M_BUTTON1 + index,
			 value);
	else if (!mouseCaptured && (index == 0))
	{
		mouseCaptured = true;
		setMouseCapture(emulation, true);
	}
	else
		send(HOST_HID_POINTER_EVENT,
			 HOST_HID_P_BUTTON1 + index,
			 value);
}

void OEHID::setMousePosition(float x, float y)
{
	if (mouseCaptured)
		return;
	
	send(HOST_HID_POINTER_EVENT,
		 HOST_HID_P_X,
		 x);
	send(HOST_HID_POINTER_EVENT,
		 HOST_HID_P_Y,
		 y);
}

void OEHID::moveMouse(float rx, float ry)
{
	if (!mouseCaptured)
		return;
	
	send(HOST_HID_MOUSE_EVENT,
		 HOST_HID_M_RX,
		 rx);
	send(HOST_HID_MOUSE_EVENT,
		 HOST_HID_M_RY,
		 ry);
}

void OEHID::sendMouseWheelEvent(int index, float value)
{
	if (!value)
		return;
	
	if (mouseCaptured)
		send(HOST_HID_MOUSE_EVENT,
			 HOST_HID_M_WX + index,
			 value);
	else
		send(HOST_HID_POINTER_EVENT,
			 HOST_HID_P_WX + index,
			 value);
}

void OEHID::setJoystickButton(int deviceIndex, int index, bool value)
{
	if (deviceIndex >= OEHID_JOYSTICKDEVICE_NUM)
		return;
	
	if (index >= OEHID_JOYSTICKBUTTON_NUM)
		return;
	
	if (joystickButtonDown[deviceIndex][index] == value)
		return;
	
	joystickButtonDown[deviceIndex][index] = value;
	
	send(HOST_HID_JOYSTICK1_EVENT + deviceIndex,
		 HOST_HID_J_BUTTON1 + index,
		 value);
}

void OEHID::setJoystickPosition(int deviceIndex, int index, float value)
{
	if (deviceIndex >= OEHID_JOYSTICKDEVICE_NUM)
		return;
	
	if (index >= OEHID_JOYSTICKAXIS_NUM)
		return;
	
	send(HOST_HID_JOYSTICK1_EVENT + deviceIndex,
		 HOST_HID_J_AXIS1 + index,
		 value);
}

void OEHID::sendJoystickHatEvent(int deviceIndex, int index, float value)
{
	if (deviceIndex >= OEHID_JOYSTICKDEVICE_NUM)
		return;
	
	if (index >= OEHID_JOYSTICKAXIS_NUM)
		return;
	
	send(HOST_HID_JOYSTICK1_EVENT + deviceIndex,
		 HOST_HID_J_AXIS1 + index,
		 value);}

void OEHID::moveJoystickBall(int deviceIndex, int index, float value)
{
}

void OEHID::setTabletButton(bool value)
{
}

void OEHID::setTabletPosition(float x, float y)
{
}

void OEHID::setTabletProximity(bool value)
{
}

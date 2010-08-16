
/**
 * OpenEmulator
 * OpenEmulator HID interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * OpenEmulator HID interface.
 */

#include "OEHID.h"

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

	string value;
//	emulation->getProperty(HOST_DEVICE, "hidMouseCapture", value);
	mouseCapture = (value == "1");
	mouseCaptured = false;
}

void OEHID::sendHIDEvent(int notification, int usageId, float value)
{
	HostHIDEvent hidEvent;
	hidEvent.usageId = usageId;
	hidEvent.value = value;
	
	if (emulation)
		emulation->notify(HOST_DEVICE, notification, &hidEvent);
	
	printf("HID event: %d %d %f\n", notification, usageId, value);
}

void OEHID::sendSystemEvent(int usageId)
{
	sendHIDEvent(HOST_HID_SYSTEM_CHANGED, usageId, 0);
}

void OEHID::setKey(int usageId, bool value)
{
	if (keyDown[usageId] == value)
		return;
	
	keyDown[usageId] = value;
	
	sendHIDEvent(HOST_HID_KEYBOARD_CHANGED, usageId, value);
	
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
	
	// Discard private usage areas
	if (((unicode < 0xe000) || (unicode > 0xf8ff)) &&
		((unicode < 0xf0000) || (unicode > 0xffffd)) &&
		((unicode < 0x100000) || (unicode > 0x10fffd)))
		sendHIDEvent(HOST_HID_UNICODEKEYBOARD_CHANGED, unicode, 0);
}

void OEHID::setMouseButton(int index, bool value)
{
	if (index >= HOST_HID_MOUSE_BUTTON_NUM)
		return;
	
	if (mouseButtonDown[index] == value)
		return;
	
	mouseButtonDown[index] = value;
	
	if (mouseCaptured)
		sendHIDEvent(HOST_HID_MOUSE_CHANGED,
					 HOST_HID_M_BUTTON1 + index,
					 value);
	else if (!mouseCaptured && mouseCapture && (index == 0))
	{
		mouseCaptured = true;
		setMouseCapture(emulation, true);
	}
	else
		sendHIDEvent(HOST_HID_POINTER_CHANGED,
					 HOST_HID_P_BUTTON1 + index,
					 value);
}

void OEHID::setMousePosition(float x, float y)
{
	if (mouseCaptured)
		return;
	
	sendHIDEvent(HOST_HID_POINTER_CHANGED,
				 HOST_HID_P_X,
				 x);
	sendHIDEvent(HOST_HID_POINTER_CHANGED,
				 HOST_HID_P_Y,
				 y);
}

void OEHID::moveMouse(float rx, float ry)
{
	if (!mouseCaptured)
		return;
	
	sendHIDEvent(HOST_HID_MOUSE_CHANGED,
				 HOST_HID_M_RX,
				 rx);
	sendHIDEvent(HOST_HID_MOUSE_CHANGED,
				 HOST_HID_M_RY,
				 ry);
}

void OEHID::sendMouseWheelEvent(int index, float value)
{
	if (!value)
		return;
	
	if (mouseCaptured)
		sendHIDEvent(HOST_HID_MOUSE_CHANGED,
					 HOST_HID_M_WX + index,
					 value);
	else
		sendHIDEvent(HOST_HID_POINTER_CHANGED,
					 HOST_HID_P_WX + index,
					 value);
}

void OEHID::setJoystickButton(int deviceIndex, int index, bool value)
{
	if (deviceIndex >= HOST_HID_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_HID_JOYSTICK_BUTTON_NUM)
		return;
	
	if (joystickButtonDown[deviceIndex][index] == value)
		return;
	
	joystickButtonDown[deviceIndex][index] = value;
	
	sendHIDEvent(HOST_HID_JOYSTICK1_CHANGED + deviceIndex,
				 HOST_HID_J_BUTTON1 + index,
				 value);
}

void OEHID::setJoystickPosition(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_HID_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_HID_JOYSTICK_AXIS_NUM)
		return;
	
	sendHIDEvent(HOST_HID_JOYSTICK1_CHANGED + deviceIndex,
				 HOST_HID_J_AXIS1 + index,
				 value);
}

void OEHID::sendJoystickHatEvent(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_HID_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_HID_JOYSTICK_HAT_NUM)
		return;
	
	sendHIDEvent(HOST_HID_JOYSTICK1_CHANGED + deviceIndex,
				 HOST_HID_J_AXIS1 + index,
				 value);
}

void OEHID::moveJoystickBall(int deviceIndex, int index, float value)
{
	if (deviceIndex >= HOST_HID_JOYSTICK_NUM)
		return;
	
	if (index >= HOST_HID_JOYSTICK_RAXIS_NUM)
		return;
	
	sendHIDEvent(HOST_HID_JOYSTICK1_CHANGED + deviceIndex,
				 HOST_HID_J_AXIS1 + index,
				 value);
}

void OEHID::setTabletButton(int index, bool value)
{
	if (index >= HOST_HID_TABLET_BUTTON_NUM)
		return;
	
	if (tabletButtonDown[index] == value)
		return;
	
	tabletButtonDown[index] = value;
	
	sendHIDEvent(HOST_HID_TABLET_CHANGED,
				 HOST_HID_J_BUTTON1 + index,
				 value);
}

void OEHID::setTabletPosition(float x, float y)
{
	sendHIDEvent(HOST_HID_TABLET_CHANGED,
				 HOST_HID_T_X,
				 x);
	sendHIDEvent(HOST_HID_TABLET_CHANGED,
				 HOST_HID_T_Y,
				 y);
}

void OEHID::setTabletProximity(bool value)
{
	sendHIDEvent(HOST_HID_TABLET_CHANGED,
				 HOST_HID_T_PROXIMITY,
				 value);
}

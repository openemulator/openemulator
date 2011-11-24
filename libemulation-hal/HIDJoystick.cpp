
/**
 * libemulation
 * HID Joystick
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a joystick component
 */

#include "HIDJoystick.h"

#import "JoystickInterface.h"

HIDJoystick::HIDJoystick()
{
    joystickNum = 0;
}

bool HIDJoystick::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case JOYSTICK_GET_JOYSTICKNUM:
            *((OEUInt32 *)data) = joystickNum;
            
            return true;
    }
    
    return false;
}

void HIDJoystick::addDevice()
{
    joystickNum++;
    
    postNotification(this, JOYSTICK_WAS_ADDED, &joystickNum);
}

void HIDJoystick::removeDevice()
{
    if (!joystickNum)
        return;
    
    joystickNum--;
    
    postNotification(this, JOYSTICK_WAS_REMOVED, &joystickNum);
}

void HIDJoystick::setAxis(int deviceIndex, int index, float value)
{
    if (index >= JOYSTICK_AXIS_NUM)
        return;
    
    JoystickHIDNotification hidNotification;
    hidNotification.deviceId = deviceIndex;
    hidNotification.usageId = JOYSTICK_AXIS1 + index;
    hidNotification.value = value;
    
    postNotification(this, JOYSTICK_DID_CHANGE, &hidNotification);
}

void HIDJoystick::setHat(int deviceIndex, int index, int value)
{
    if (index >= JOYSTICK_HAT_NUM)
        return;
    
    JoystickHIDNotification hidNotification;
    hidNotification.deviceId = deviceIndex;
    hidNotification.usageId = JOYSTICK_HAT1 + index;
    hidNotification.value = value;
    
    postNotification(this, JOYSTICK_DID_CHANGE, &hidNotification);
}

void HIDJoystick::setButton(int deviceIndex, int index, bool value)
{
    if (index >= JOYSTICK_BUTTON_NUM)
        return;
    
    JoystickHIDNotification hidNotification;
    hidNotification.deviceId = deviceIndex;
    hidNotification.usageId = JOYSTICK_BUTTON1 + index;
    hidNotification.value = value;
    
    postNotification(this, JOYSTICK_DID_CHANGE, &hidNotification);
}

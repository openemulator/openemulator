
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
            *((OEInt *)data) = joystickNum;
            
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

void HIDJoystick::setAxis(OEInt deviceIndex, OEInt index, float value)
{
    if (index >= JOYSTICK_AXIS_NUM)
        return;
    
    JoystickHIDEvent hidEvent;
    hidEvent.deviceId = deviceIndex;
    hidEvent.usageId = JOYSTICK_AXIS1 + index;
    hidEvent.value = value;
    
    postNotification(this, JOYSTICK_DID_CHANGE, &hidEvent);
}

void HIDJoystick::setHat(OEInt deviceIndex, OEInt index, OEInt value)
{
    if (index >= JOYSTICK_HAT_NUM)
        return;
    
    JoystickHIDEvent hidEvent;
    hidEvent.deviceId = deviceIndex;
    hidEvent.usageId = JOYSTICK_HAT1 + index;
    hidEvent.value = value;
    
    postNotification(this, JOYSTICK_DID_CHANGE, &hidEvent);
}

void HIDJoystick::setButton(OEInt deviceIndex, OEInt index, bool value)
{
    if (index >= JOYSTICK_BUTTON_NUM)
        return;
    
    JoystickHIDEvent hidEvent;
    hidEvent.deviceId = deviceIndex;
    hidEvent.usageId = JOYSTICK_BUTTON1 + index;
    hidEvent.value = value;
    
    postNotification(this, JOYSTICK_DID_CHANGE, &hidEvent);
}

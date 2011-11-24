
/**
 * libemulation
 * HID Joystick
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a joystick component
 */

#include "OEComponent.h"

class HIDJoystick : public OEComponent
{
public:
    HIDJoystick();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void addDevice();
    void removeDevice();
    
    void setAxis(int deviceIndex, int index, float value);
    void setHat(int deviceIndex, int index, int value);
    void setButton(int deviceIndex, int index, bool value);
    
private:
    OEUInt32 joystickNum;
};

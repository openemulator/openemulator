
/**
 * libemulator
 * Apple II Game Port 
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple II Game Port
 */

#include "AppleIIGamePort.h"

#include "ControlBusInterface.h"
#include "JoystickInterface.h"
#include "AppleIIInterface.h"

AppleIIGamePort::AppleIIGamePort()
{
    controlBus = NULL;
    floatingBus = NULL;
    gamePort = NULL;
    
    for (OEInt i = 0; i < 4; i++)
        setPDL(i, 0.5F);
    
    for (OEInt i = 0; i < 4; i++)
        pb[i] = 0;
    
    for (OEInt i = 0; i < 4; i++)
        an[i] = false;
    
    timerStart = 0;
}

bool AppleIIGamePort::setValue(string name, string value)
{
	if (name == "an0")
		an[0] = getOEInt(value);
	else if (name == "an1")
		an[1] = getOEInt(value);
	else if (name == "an2")
		an[2] = getOEInt(value);
	else if (name == "an3")
		an[3] = getOEInt(value);
	else
		return false;
	
	return true;
}

bool AppleIIGamePort::getValue(string name, string& value)
{
	if (name == "an0")
        value = getString(an[0]);
	else if (name == "an1")
        value = getString(an[1]);
	else if (name == "an2")
        value = getString(an[2]);
	else if (name == "an3")
        value = getString(an[3]);
	else
		return false;
	
	return true;
}

bool AppleIIGamePort::setRef(string name, OEComponent *ref)
{
	if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
		controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
    }
	else if (name == "floatingBus")
		floatingBus = ref;
    else if (name == "gamePort")
    {
        if (gamePort)
            gamePort->removeObserver(this, JOYSTICK_DID_CHANGE);
        gamePort = ref;
        if (gamePort)
            gamePort->addObserver(this, JOYSTICK_DID_CHANGE);
    }
	else
		return false;
	
	return true;
}

bool AppleIIGamePort::init()
{
    OECheckComponent(controlBus);
    OECheckComponent(floatingBus);
    
    return true;
}

bool AppleIIGamePort::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case APPLEII_SET_PDL0:
        case APPLEII_SET_PDL1:
        case APPLEII_SET_PDL2:
        case APPLEII_SET_PDL3:
            pb[message - APPLEII_SET_PDL0] = *((float *)data);
            
            break;
            
        case APPLEII_SET_PB3:
        case APPLEII_SET_PB0:
        case APPLEII_SET_PB1:
        case APPLEII_SET_PB2:
            pb[message - APPLEII_SET_PB3] = *((bool *)data);
            
            break;
            
        case APPLEII_GET_AN0:
        case APPLEII_GET_AN1:
        case APPLEII_GET_AN2:
        case APPLEII_GET_AN3:
            *((bool *)data) = an[message - APPLEII_GET_AN0];
            
            return true;
    }
    
    return false;
}

void AppleIIGamePort::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == gamePort)
    {
        if (notification == JOYSTICK_DID_CHANGE)
        {
            JoystickHIDEvent *hidEvent = (JoystickHIDEvent *)data;
            
            switch (hidEvent->usageId)
            {
                case JOYSTICK_AXIS1:
                case JOYSTICK_AXIS2:
                case JOYSTICK_AXIS3:
                case JOYSTICK_AXIS4:
                    setPDL(hidEvent->usageId - JOYSTICK_AXIS1, hidEvent->value);
                    
                    break;
                    
                case JOYSTICK_BUTTON1:
                case JOYSTICK_BUTTON2:
                case JOYSTICK_BUTTON3:
                    setPB(hidEvent->usageId - JOYSTICK_BUTTON1 + 1, hidEvent->value ? true : false);
                    
                    break;
            }
        }
    }
    else if (sender == controlBus)
    {
        ControlBusPowerState powerState = *((ControlBusPowerState *)data);
        
        if (powerState == CONTROLBUS_POWERSTATE_OFF)
        {
            setAN(0, false);
            setAN(1, false);
            setAN(2, false);
            setAN(3, false);
        }
    }
}

OEChar AppleIIGamePort::read(OEAddress address)
{
    OEChar value = floatingBus->read(address);
    
    switch (address & 0x3f)
    {
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0a: case 0x0b:
        case 0x0c: case 0x0d: case 0x0e: case 0x0f:
            postNotification(this, APPLEII_DID_STROBE, NULL);
            
            break;
            
        case 0x18: case 0x19: case 0x1a: case 0x1b:
        case 0x1c: case 0x1d: case 0x1e: case 0x1f:
            setAN((address & 0x6) >> 1, (address & 0x1));
            
            break;
            
        case 0x20: case 0x21: case 0x22: case 0x23:
        case 0x28: case 0x29: case 0x2a: case 0x2b:
            value = (pb[address & 0x3] << 7) | (value & 0x7f);
            
            break;
            
        case 0x24: case 0x25: case 0x26: case 0x27:
        case 0x2c: case 0x2d: case 0x2e: case 0x2f:
            value = (isTimerPending(address & 0x3) << 7) | (value & 0x7f);
            
            break;
            
        case 0x30: case 0x31: case 0x32: case 0x33:
        case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b:
        case 0x3c: case 0x3d: case 0x3e: case 0x3f:
            resetTimer();
            
            break;
    }
    
    return value;
}

void AppleIIGamePort::write(OEAddress address, OEChar value)
{
    switch (address & 0x3f)
    {
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x04: case 0x05: case 0x06: case 0x07:
        case 0x08: case 0x09: case 0x0a: case 0x0b:
        case 0x0c: case 0x0d: case 0x0e: case 0x0f:
            postNotification(this, APPLEII_DID_STROBE, NULL);
            
            break;
            
        case 0x18: case 0x19: case 0x1a: case 0x1b:
        case 0x1c: case 0x1d: case 0x1e: case 0x1f:
            setAN((address & 0x6) >> 1, (address & 0x1));
            
            break;
            
        case 0x30: case 0x31: case 0x32: case 0x33:
        case 0x34: case 0x35: case 0x36: case 0x37:
        case 0x38: case 0x39: case 0x3a: case 0x3b:
        case 0x3c: case 0x3d: case 0x3e: case 0x3f:
            resetTimer();
            
            break;
    }
}

void AppleIIGamePort::setAN(OELong index, bool value)
{
    bool oldValue = an[index];
    
    an[index] = value;
    
    if (value != oldValue)
        postNotification(this, (int) (APPLEII_AN0_DID_CHANGE + index), &value);
}

void AppleIIGamePort::setPDL(OELong index, float value)
{
    if (value < 0)
        value = 0;
    else if (value > 1)
        value = 1;
    
    value *= 255;
    
    // From Applesoft BASIC, $FB1E-$FB2E
    pdl[index] = value * 11 + 8;
}

void AppleIIGamePort::setPB(OELong index, bool value)
{
    pb[index] = value;
}

bool AppleIIGamePort::isTimerPending(OELong index)
{
    OELong timerCount;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &timerCount);
    
    return ((timerCount - timerStart) < pdl[index]);
}

void AppleIIGamePort::resetTimer()
{
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &timerStart);
}

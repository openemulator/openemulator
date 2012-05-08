
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
        setPDL(i, 0.5);
    
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
		controlBus = ref;
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
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
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

OEChar AppleIIGamePort::read(OEAddress address)
{
    OEChar value = floatingBus->read(address);
    
    switch (address & 0x7f)
    {
        case 0x40: case 0x41: case 0x42: case 0x43:
        case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4a: case 0x4b:
        case 0x4c: case 0x4d: case 0x4e: case 0x4f:
            postNotification(this, APPLEII_DID_STROBE, NULL);
            
            break;
            
        case 0x58: case 0x59: case 0x5a: case 0x5b:
        case 0x5c: case 0x5d: case 0x5e: case 0x5f:
            setAN((address & 0x6) >> 1, (address & 0x1));
            
            break;
            
        case 0x60: case 0x61: case 0x62: case 0x63:
        case 0x68: case 0x69: case 0x6a: case 0x6b:
            value = (pb[address & 0x3] << 7) | (value & 0x7f);
            
            break;
            
        case 0x64: case 0x65: case 0x66: case 0x67:
        case 0x6c: case 0x6d: case 0x6e: case 0x6f:
            value = (isTimerPending(address & 0x3) << 7) | (value & 0x7f);
            
            break;
            
        case 0x70: case 0x71: case 0x72: case 0x73:
        case 0x74: case 0x75: case 0x76: case 0x77:
        case 0x78: case 0x79: case 0x7a: case 0x7b:
        case 0x7c: case 0x7d: case 0x7e: case 0x7f:
            resetTimer();
            
            break;
    }
    
    return value;
}

void AppleIIGamePort::write(OEAddress address, OEChar value)
{
    switch (address & 0x7f)
    {
        case 0x40: case 0x41: case 0x42: case 0x43:
        case 0x44: case 0x45: case 0x46: case 0x47:
        case 0x48: case 0x49: case 0x4a: case 0x4b:
        case 0x4c: case 0x4d: case 0x4e: case 0x4f:
            postNotification(this, APPLEII_DID_STROBE, NULL);
            
            break;
            
        case 0x58: case 0x59: case 0x5a: case 0x5b:
        case 0x5c: case 0x5d: case 0x5e: case 0x5f:
            setAN((address & 0x6) >> 1, (address & 0x1));
            
            break;
            
        case 0x70: case 0x71: case 0x62: case 0x63:
        case 0x74: case 0x75: case 0x66: case 0x67:
        case 0x78: case 0x79: case 0x6a: case 0x6b:
        case 0x7c: case 0x6d: case 0x6e: case 0x6f:
            resetTimer();
            
            break;
    }
}

void AppleIIGamePort::setAN(OEInt index, bool value)
{
    bool oldValue = an[index];
    
    an[index] = value;
    
    if (value != oldValue)
        postNotification(this, APPLEII_AN0_DID_CHANGE + index, &value);
}

void AppleIIGamePort::setPDL(OEInt index, float value)
{
    if (value < 0)
        value = 0;
    else if (value > 1)
        value = 1;
    
    value *= 255;
    
    // From Applesoft BASIC, $FB1E-$FB2E
    pdl[index] = value * 11 + 8;
}

void AppleIIGamePort::setPB(OEInt index, bool value)
{
    pb[index] = value;
}

bool AppleIIGamePort::isTimerPending(OEInt index)
{
    OELong timerCount;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &timerCount);
    
    return ((timerCount - timerStart) < pdl[index]);
}

void AppleIIGamePort::resetTimer()
{
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &timerStart);
}

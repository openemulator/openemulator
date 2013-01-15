
/**
 * libemulation
 * Apple III Game Port
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple III game port
 */

#include "AppleIIIGamePort.h"

#include "ControlBusInterface.h"
#include "JoystickInterface.h"
#include "AppleIIInterface.h"

AppleIIIGamePort::AppleIIIGamePort()
{
}

OEChar AppleIIIGamePort::read(OEAddress address)
{
    OEChar value = floatingBus->read(address);
    
    switch (address & 0x1f)
    {
        case 0x00: case 0x01: case 0x02: case 0x03:
        case 0x08: case 0x09: case 0x0a: case 0x0b:
            value = (pb[address & 0x3] << 7) | (value & 0x7f);
            
            break;
            
        case 0x04: case 0x05:
        case 0x0c: case 0x0d:
            // /IRQ1 and /IRQ2
            value = 0x80 | (value & 0x7f);
            
            break;
            
        case 0x06:
        case 0x0e:
            value = (!isTimerPending() << 7) | (value & 0x7f);
            
            break;
            
        case 0x07:
        case 0x0f:
            // MUX1
            value = 0x00 | (value & 0x7f);
            
            break;
    }
    
    return value;
}

void AppleIIIGamePort::write(OEAddress address, OEChar value)
{
    switch (address & 0x1f)
    {
        case 0x18: case 0x19:
            setChannelSelect((1 << 0), address & 0x1);
            
            break;
            
        case 0x1a: case 0x1b:
            setChannelSelect((1 << 2), address & 0x1);
            
            break;
            
        case 0x1c: case 0x1d:
            resetTimer();
            
            break;
            
        case 0x1e: case 0x1f:
            setChannelSelect((1 << 1), address & 0x1);
            
            break;
    }
}

void AppleIIIGamePort::setChannelSelect(OEInt index, bool value)
{
    OESetBit(channelSelect, 1 << index, value);
}

bool AppleIIIGamePort::isTimerPending()
{
    OELong timerCount;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &timerCount);
    
    // Return timer overflow from UA9708 channel
    switch (channelSelect)
    {
        case 0:
            // GND reference
            return false;
            
        case 1: case 2: case 3: case 4:
            return ((timerCount - timerStart) < pdl[channelSelect - 1]);
            
        case 5:
            // Battery
            return ((timerCount - timerStart) < 0.5F * APPLEIIGAMEPORT_MAXVALUE);
            
        case 6:
            // Not connected
            return false;
            
        case 7:
            // VREF reference
            return ((timerCount - timerStart) < APPLEIIGAMEPORT_MAXVALUE);
    }
    
    return false;
}


/**
 * libemulation
 * Floating bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic floating bus
 */

#include "stdlib.h"
#include "FloatingBus.h"

FloatingBus::FloatingBus()
{
    randomMode = false;
    
    busValue = 0;
}

bool FloatingBus::setValue(string name, string value)
{
    if (name == "mode")
    {
        if (value == "vcc")
            busValue = 0xff;
        else
            busValue = 0x00;
        
        randomMode = (value == "random");
    }
    else
        return false;
    
    return true;
}

OEUInt8 FloatingBus::read(OEAddress address)
{
    if (randomMode)
        return random() & 0xff;
    else
        return busValue;
}

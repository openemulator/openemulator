
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include "AppleIIAudioOut.h"

#include "ControlBusInterface.h"
#include "AppleIIInterface.h"

AppleIIAudioOut::AppleIIAudioOut() : Audio1Bit()
{
    controlBus = NULL;
    floatingBus = NULL;
    
    cassetteOut = false;
    
    lastCycles = 0;
    relaxationState = false;
}

bool AppleIIAudioOut::setValue(string name, string value)
{
    if (name == "cassetteOut")
        cassetteOut = getOEInt(value);
    else
        return Audio1Bit::setValue(name, value);
    
    return true;
}

bool AppleIIAudioOut::getValue(string name, string &value)
{
    if (name == "cassetteOut")
        value = getString(cassetteOut);
    else
        return Audio1Bit::getValue(name, value);
    
    return true;
}

bool AppleIIAudioOut::setRef(string name, OEComponent *id)
{
	if (name == "controlBus")
		controlBus = id;
	else if (name == "floatingBus")
		floatingBus = id;
	else
		return Audio1Bit::setRef(name, id);
	
	return true;
}

bool AppleIIAudioOut::init()
{
    if (!Audio1Bit::init())
        return false;
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    return true;
}

OEChar AppleIIAudioOut::read(OEAddress address)
{
    write(address, 0);
    
	return floatingBus->read(address);
}

void AppleIIAudioOut::write(OEAddress address, OEChar value)
{
    if (!(address & 0x0010) && !cassetteOut)
        return;
    
    if (controlBus)
    {
        OELong cycles;
        
        controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
        
        OELong deltaCycles = cycles - lastCycles;
        lastCycles = cycles;
        
        if (deltaCycles > 0.05 * APPLEII_CLOCKFREQUENCY)
        {
            relaxationState = !relaxationState;
            
            if (relaxationState)
                return;
        }
    }
    
    toggleAudioOutput();
}

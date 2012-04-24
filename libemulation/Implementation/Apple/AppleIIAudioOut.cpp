
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include "AppleIIAudioOut.h"

AppleIIAudioOut::AppleIIAudioOut() : Audio1Bit()
{
    floatingBus = NULL;
}

bool AppleIIAudioOut::setRef(string name, OEComponent *id)
{
	if (name == "floatingBus")
		floatingBus = id;
	else
		return Audio1Bit::setRef(name, id);
	
	return true;
}

bool AppleIIAudioOut::init()
{
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    return Audio1Bit::init();
}

OEChar AppleIIAudioOut::read(OEAddress address)
{
    toggleAudioOutput();
    
	return floatingBus->read(address);
}

void AppleIIAudioOut::write(OEAddress address, OEChar value)
{
    toggleAudioOutput();
}

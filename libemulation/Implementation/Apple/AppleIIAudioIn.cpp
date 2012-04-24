
/**
 * libemulator
 * Apple II Audio Input
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio input
 */

#include "AppleIIAudioIn.h"

AppleIIAudioIn::AppleIIAudioIn() : Audio1Bit()
{
    floatingBus = NULL;
}

bool AppleIIAudioIn::setRef(string name, OEComponent *ref)
{
	if (name == "floatingBus")
        floatingBus = ref;
	else
		return Audio1Bit::setRef(name, ref);
	
	return true;
}

bool AppleIIAudioIn::init()
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

OEChar AppleIIAudioIn::read(OEAddress address)
{
    return (readAudioInput() << 7) | (floatingBus->read(0) & 0x7f);
}

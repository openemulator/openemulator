
/**
 * libemulator
 * Apple II Audio Input
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio input
 */

#include "AppleIIAudioIn.h"

#define SCHMITT_TRIGGER_THRESHOLD 4

AppleIIAudioIn::AppleIIAudioIn()
{
    audioCodec = NULL;
    floatingBus = NULL;
    
    threshold = 0x80;
}

bool AppleIIAudioIn::setRef(string name, OEComponent *ref)
{
	if (name == "audioCodec")
		audioCodec = ref;
	else if (name == "floatingBus")
		floatingBus = ref;
	else
		return false;
	
	return true;
}

bool AppleIIAudioIn::init()
{
    if (!audioCodec)
    {
        logMessage("audioCodec not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    return true;
}

OEUInt8 AppleIIAudioIn::read(OEAddress address)
{
    // A Schmitt trigger to improve noise rejection...
    if (audioCodec->read(0) >= threshold)
    {
        threshold = 0x80 - SCHMITT_TRIGGER_THRESHOLD;
        
        return (floatingBus->read(0) & 0x7f) | 0x80;
    }
    else
    {
        threshold = 0x80 + SCHMITT_TRIGGER_THRESHOLD;
        
        return (floatingBus->read(0) & 0x7f) | 0x00;
    }
}


/**
 * libemulator
 * Apple II Audio Input
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio input
 */

#include "AppleIIAudioIn.h"

AppleIIAudioIn::AppleIIAudioIn()
{
    audioCodec = NULL;
    floatingBus = NULL;
    
    noiseRejection = 5;
    threshold = 0x80;
}

bool AppleIIAudioIn::setValue(string name, string value)
{
    if (name == "noiseRejection")
        noiseRejection = getInt(value);
    else
        return false;
    
    return true;
}

bool AppleIIAudioIn::getValue(string name, string &value)
{
    if (name == "noiseRejection")
        value = getString(noiseRejection);
    else
        return false;
    
    return true;
}

bool AppleIIAudioIn::setRef(string name, OEComponent *ref)
{
	if (name == "floatingBus")
		floatingBus = ref;
	else if (name == "audioCodec")
		audioCodec = ref;
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
        threshold = 0x80 - noiseRejection;
        
        return 0x80 | (floatingBus->read(0) & 0x7f);
    }
    else
    {
        threshold = 0x80 + noiseRejection;
        
        return 0x00 | (floatingBus->read(0) & 0x7f);
    }
}


/**
 * libemulator
 * Apple II Audio Input
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio input
 */

#include "AppleIIAudioIn.h"

AppleIIAudioIn::AppleIIAudioIn()
{
    noiseRejection = 0.04;
    
    audioCodec = NULL;
    floatingBus = NULL;
    
    inputCurrentThreshold = 0x80;
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
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    if (!audioCodec)
    {
        logMessage("audioCodec not connected");
        
        return false;
    }
    
    return true;
}

void AppleIIAudioIn::update()
{
    inputTriggerThreshold = 127 * noiseRejection;
}

OEUInt8 AppleIIAudioIn::read(OEAddress address)
{
    // A Schmitt trigger to improve noise rejection...
    if (audioCodec->read(0) >= inputCurrentThreshold)
    {
        inputCurrentThreshold = 0x80 - inputTriggerThreshold;
        
        return 0x80 | (floatingBus->read(0) & 0x7f);
    }
    else
    {
        inputCurrentThreshold = 0x80 + inputTriggerThreshold;
        
        return 0x00 | (floatingBus->read(0) & 0x7f);
    }
}

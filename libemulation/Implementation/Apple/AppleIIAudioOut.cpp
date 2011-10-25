
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include "AppleIIAudioOut.h"

AppleIIAudioOut::AppleIIAudioOut()
{
    audioCodec = NULL;
    floatingBus = NULL;
    
    audioLevel = 0x80;
}

bool AppleIIAudioOut::setRef(string name, OEComponent *id)
{
	if (name == "audioCodec")
		audioCodec = id;
	else if (name == "floatingBus")
		floatingBus = id;
	else
		return false;
	
	return true;
}

bool AppleIIAudioOut::init()
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

OEUInt8 AppleIIAudioOut::read(OEAddress address)
{
    toggleSpeaker();
    
	return floatingBus->read(address);
}

void AppleIIAudioOut::write(OEAddress address, OEUInt8 value)
{
    toggleSpeaker();
}

void AppleIIAudioOut::toggleSpeaker()
{
    audioLevel = (audioLevel == 0x80) ? 0xc0 : 0x80;
    
    audioCodec->write(0, audioLevel);
    audioCodec->write(1, audioLevel);
}

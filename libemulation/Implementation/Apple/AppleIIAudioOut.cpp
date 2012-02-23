
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include <math.h>

#include "AppleIIAudioOut.h"

AppleIIAudioOut::AppleIIAudioOut()
{
    volume = 1;
    
    audioCodec = NULL;
    floatingBus = NULL;
    
    outputState = false;
}

bool AppleIIAudioOut::setValue(string name, string value)
{
    if (name == "volume")
        volume = getFloat(value);
    else
        return false;
    
    return true;
}

bool AppleIIAudioOut::getValue(string name, string& value)
{
    if (name == "volume")
        value = getString(volume);
    else
        return false;
    
    return true;
}

bool AppleIIAudioOut::setRef(string name, OEComponent *id)
{
	if (name == "floatingBus")
		floatingBus = id;
	else if (name == "audioCodec")
		audioCodec = id;
	else
		return false;
	
	return true;
}

bool AppleIIAudioOut::init()
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

void AppleIIAudioOut::update()
{
    if (volume != 0)
        outputHighLevel = 16384 * pow(10.0, (volume - 1.0) * 40.0 / 20.0);
    else
        outputHighLevel = 0;
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
    outputState = !outputState;
    
    // Stereo
    if (outputState)
    {
        audioCodec->write16(0, outputHighLevel);
        audioCodec->write16(1, outputHighLevel);
    }
    else
    {
        audioCodec->write16(0, 0);
        audioCodec->write16(1, 0);
    }
}

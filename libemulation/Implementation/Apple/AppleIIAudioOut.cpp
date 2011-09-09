
/**
 * libemulator
 * Apple II Audio Output
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II audio output
 */

#include "AppleIIAudioOut.h"

bool AppleIIAudioOut::setValue(string name, string value)
{
	if (name == "state")
		state = (OEUInt32) getUInt(value);
	else
		return false;
	
	return true;
}

bool AppleIIAudioOut::setRef(string name, OEComponent *id)
{
	if (name == "sampleConverter")
		sampleConverter = id;
	else if (name == "floatingBus")
		floatingBus = id;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIIAudioOut::read(OEAddress address)
{
	return floatingBus->read(0);
}

void AppleIIAudioOut::write(OEAddress address, OEUInt8 value)
{
}

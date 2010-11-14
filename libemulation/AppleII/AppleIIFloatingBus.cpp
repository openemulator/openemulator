
/**
 * libemulator
 * Apple II Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II floating bus.
 */

#include "AppleIIFloatingBus.h"

bool AppleIIFloatingBus::setRef(string name, OEComponent *id)
{
	if (name == "controlBus")
		controlBus = id;
	else if (name == "ram")
		ram = id;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIIFloatingBus::read(OEAddress address)
{
	return 0;
}

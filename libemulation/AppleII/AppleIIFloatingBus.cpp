
/**
 * libemulator
 * Apple II Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II floating bus.
 */

#include "AppleIIFloatingBus.h"

bool AppleIIFloatingBus::setComponent(string name, OEComponent *component)
{
	if (name == "controlBus")
		controlBus = component;
	else if (name == "ram")
		ram = component;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIIFloatingBus::read(OEAddress address)
{
	return 0;
}

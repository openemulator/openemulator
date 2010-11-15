
/**
 * libemulator
 * Apple II Game Port
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II's game port.
 */

#include "AppleIIGamePort.h"

bool AppleIIGamePort::setValue(string name, string value)
{
	if (name == "an0")
		an0 = getInt(value);
	else if (name == "an1")
		an1 = getInt(value);
	else if (name == "an2")
		an2 = getInt(value);
	else if (name == "an3")
		an3 = getInt(value);
	else
		return false;
	
	return true;
}

bool AppleIIGamePort::setRef(string name, OEComponent *ref)
{
	if (name == "floatingBus")
		floatingBus = ref;
	else if (name == "port")
		port = ref;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIIGamePort::read(OEAddress address)
{
	return floatingBus->read(address);
}

void AppleIIGamePort::write(OEAddress address, OEUInt8 value)
{
}


/**
 * libemulation
 * Apple I IO
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Input Output range
 */

#include "AppleIIO.h"

#define APPLEIIO_MASK	0x10

bool AppleIIO::setProperty(const string &name, const string &value)
{
	if (name == "map")
		mappedRange = value;
	else
		return false;
	
	return true;
}

bool AppleIIO::connect(const string &name, OEComponent *component)
{
	if (name == "pia")
		pia = component;
	else if (name == "floatingBus")
		floatingBus = component;
	else
		return false;
	
	return true;
}

bool AppleIIO::getMemoryMap(string &range)
{
	range = mappedRange;
	
	return true;
}

OEUInt8 AppleIIO::read(int address)
{
	if (address & APPLEIIO_MASK)
		return pia->read(address);
	else
		return floatingBus->read(address);
}

void AppleIIO::write(int address, OEUInt8 value)
{
	if (address & APPLEIIO_MASK)
		pia->write(address, value);
	else
		floatingBus->write(address, value);
}

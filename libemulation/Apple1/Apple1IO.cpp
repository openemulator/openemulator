
/**
 * libemulation
 * Apple I IO
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Input Output range
 */

#include "Apple1IO.h"

#define APPLE1IO_MASK	0x10

bool Apple1IO::setProperty(const string &name, const string &value)
{
	if (name == "map")
		mappedRange = value;
	else
		return false;
	
	return true;
}

bool Apple1IO::connect(const string &name, OEComponent *component)
{
	if (name == "pia")
		pia = component;
	else if (name == "floatingBus")
		floatingBus = component;
	else
		return false;
	
	return true;
}

bool Apple1IO::getMemoryMap(string &range)
{
	range = mappedRange;
	
	return true;
}

OEUInt8 Apple1IO::read(int address)
{
	if (address & APPLE1IO_MASK)
		return pia->read(address);
	else
		return floatingBus->read(address);
}

void Apple1IO::write(int address, OEUInt8 value)
{
	if (address & APPLE1IO_MASK)
		pia->write(address, value);
	else
		floatingBus->write(address, value);
}

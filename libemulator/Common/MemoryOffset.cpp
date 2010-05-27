
/**
 * libemulator
 * Generic memory offset logic
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic memory offset logic
 */

#include "MemoryOffset.h"

MemoryOffset::MemoryOffset()
{
	offset = 0;
}

bool MemoryOffset::setProperty(string name, string value)
{
	if (name == "map")
		mappedRange = value;
	else if (name == "offset")
		offset = getInt(value);
	else
		return false;
	
	return true;
}

bool MemoryOffset::connect(string name, OEComponent *component)
{
	if (name == "component")
		connectedComponent = component;
	else
		return false;
	
	return true;
}

bool MemoryOffset::getMemoryMap(string &range)
{
	range = mappedRange;
	
	return true;
}

int MemoryOffset::read(int address)
{
	return connectedComponent->read(address + offset);
}

void MemoryOffset::write(int address, int value)
{
	connectedComponent->write(address + offset, value);
}

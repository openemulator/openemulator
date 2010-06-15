
/**
 * libemulation
 * Apple I Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Floating Bus
 */

#include "AppleIFloatingBus.h"

bool AppleIFloatingBus::connect(const string &name, OEComponent *component)
{
	if (name == "system")
		system = component;
	else if (name == "ram1")
		memory = component;
	else if (name == "ram2")
		memory = component;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIFloatingBus::read(int address)
{
	address = 0;
	
	// To-Do: Ask system for current time, calculate last refresh byte.
	
	return memory->read(address);
}

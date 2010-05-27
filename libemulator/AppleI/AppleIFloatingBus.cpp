
/**
 * libemulator
 * Apple I Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Floating Bus
 */

#include "AppleIFloatingBus.h"

bool AppleIFloatingBus::connect(string name, OEComponent *component)
{
	if (name == "system")
		system = component;
	else if (name == "memory")
		memory = component;
	else
		return false;
	
	return true;
}

int AppleIFloatingBus::read(int address)
{
	address = 0;
	
	// To-Do: Ask system for current time, calculate last refresh byte.
	
	return memory->read(address);
}

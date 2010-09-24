
/**
 * libemulation
 * Floating bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic floating bus
 */

#include "stdlib.h"
#include "FloatingBus.h"

FloatingBus::FloatingBus()
{
	busValue = 0;
	randomValue = false;
}

bool FloatingBus::setValue(string name, string value)
{
	if (name == "randomValue")
		randomValue = getInt(value);
	else if (name == "busValue")
		busValue = getInt(value);
	else
		return false;
	
	return true;
}

OEUInt8 FloatingBus::read(int address)
{
	if (randomValue)
		return random() & 0xff;
	else
		return busValue;
}

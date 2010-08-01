
/**
 * libemulation
 * Floating bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Manages a floating bus
 */

#include "FloatingBus.h"

FloatingBus::FloatingBus()
{
	busValue = 0xff;
}

bool FloatingBus::setProperty(const string &name, const string &value)
{
	if (name == "busValue")
		busValue = getInt(value);
	else
		return false;
	
	return true;
}

OEUInt8 FloatingBus::read(int address)
{
	return busValue;
}

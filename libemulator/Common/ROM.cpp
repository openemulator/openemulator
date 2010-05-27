
/**
 * libemulator
 * Generic ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic ROM segment
 */

#include "ROM.h"

ROM::ROM()
{
	mask = 0;
	
	memory.resize(1);
}

bool ROM::setProperty(string name, string &value)
{
	if (name == "map")
		mappedRange = value;
	else
		return false;
	
	return true;
}

bool ROM::setResource(string name, OEData &data)
{
	if (name == "image")
	{
		memory = data;
		int size = getLowerPowerOf2(memory.size());
		if (size < 1)
			size = 1;
		memory.resize(size);
		mask = size - 1;
	}
	else
		return false;
	
	return true;
}

bool ROM::getMemoryMap(string &range)
{
	range = mappedRange;
	
	return true;
}

int ROM::read(int address)
{
	return memory[address & mask];
}

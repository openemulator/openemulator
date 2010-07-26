
/**
 * libemulation
 * Generic ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic ROM segment
 */

#include "ROM.h"

ROM::ROM()
{
	memory = new OEData();
	updateMemory(1);
}

ROM::~ROM()
{
	delete memory;
}

void ROM::updateMemory(int size)
{
	size = getNextPowerOf2(size);
	if (size < 1)
		size = 1;
	memory->resize(size);
	mask = size - 1;
	data = &memory->front();
}

bool ROM::setProperty(const string &name, const string &value)
{
	if (name == "map")
		mappedRange = value;
	else
		return false;
	
	return true;
}

bool ROM::setResource(const string &name, OEData *data)
{
	if (name == "image")
	{
		delete memory;
		memory = data;
		
		updateMemory(memory->size());
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

OEUInt8 ROM::read(int address)
{
	return data[address & mask];
}

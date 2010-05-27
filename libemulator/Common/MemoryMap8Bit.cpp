
/**
 * libemulator
 * 8 bit memory map
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic 8-bit memory map
 */

#include <iostream>

#include "MemoryMap8Bit.h"

bool MemoryMap8Bit::setProperty(string name, string value)
{
	if (name == "map")
		mappedRange = value;
	else
		return false;
	
	return true;
}

bool MemoryMap8Bit::connect(string name, OEComponent *component)
{
	string range;
	component->getMemoryMap(range);
	
	setMemoryMap(component, range);
	
	return true;
}

bool MemoryMap8Bit::setMemoryMap(OEComponent *component, string ranges)
{
	OEMemoryRanges memoryRanges = getRanges(ranges);
	
	for (OEMemoryRanges::iterator i = memoryRanges.begin();
		 i != memoryRanges.end();
		 i++)
	{
		if (i->end >= MEMORYMAP8BIT_SIZE)
		{
			OELog("");
			return false;
		}
		
		for (int n = i->start; n < i->end; n++)
		{
			if (i->read)
				readMap[i] = component;
			if (i->write)
				writeMap[i] = component;
		}
	}
	
	return true;
}

bool MemoryMap8Bit::getMemoryMap(string &range)
{
	range = mappedRange;
	
	return true;
}

int MemoryMap8Bit::read(int address)
{
	return readMap[address & 0xff]->read(address);
}

void MemoryMap8Bit::write(int address, int value)
{
	writeMap[address & 0xff]->write(address, value);
}

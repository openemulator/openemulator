
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

bool MemoryMap8Bit::setProperty(const string &name, const string &value)
{
	if (name == "map")
		mappedRange = value;
	else
		return false;
	
	return true;
}

bool MemoryMap8Bit::connect(const string &name, OEComponent *component)
{
	string range;
	component->getMemoryMap(range);
	
	setMemoryMap(component, range);
	
	return true;
}

bool MemoryMap8Bit::setMemoryMap(OEComponent *component, const string &value)
{
	OEMemoryRanges ranges;
	
	if (!getRanges(ranges, value))
		return false;
	
	for (OEMemoryRanges::iterator i = ranges.begin();
		 i != ranges.end();
		 i++)
	{
		if (i->end >= MEMORYMAP8BIT_SIZE)
		{
			OELog("memory range " + value + "invalid");
			return false;
		}
		
		for (int j = i->start; j < i->end; j++)
		{
			if (i->read)
				readMap[j] = component;
			if (i->write)
				writeMap[j] = component;
		}
	}
	
	return true;
}

bool MemoryMap8Bit::getMemoryMap(string &range)
{
	range = mappedRange;
	
	return true;
}

OEUInt8 MemoryMap8Bit::read(int address)
{
	return readMap[address & 0xff]->read(address);
}

void MemoryMap8Bit::write(int address, OEUInt8 value)
{
	writeMap[address & 0xff]->write(address, value);
}


/**
 * libemulator
 * 16 bit memory map
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic 16-bit memory map
 */

#include <iostream>

#include "MemoryMap16Bit.h"

bool MemoryMap16Bit::setProperty(const string &name, const string &value)
{
	if (name == "map")
		mappedRange = value;
	else
		return false;
	
	return true;
}

bool MemoryMap16Bit::connect(const string &name, OEComponent *component)
{
	string range;
	component->getMemoryMap(range);
	
	setMemoryMap(component, range);
	
	return true;
}

bool MemoryMap16Bit::setMemoryMap(OEComponent *component, const string &value)
{
	OEMemoryRanges ranges;
	
	if (!getRanges(ranges, value))
		return false;
	
	for (OEMemoryRanges::iterator i = ranges.begin();
		 i != ranges.end();
		 i++)
	{
		if ((i->end >= MEMORYMAP16BIT_SIZE * 0x100) ||
			(i->start & 0xff) || ((i->end & 0xff) != 0xff))
		{
			OELog("memory range " + value + "invalid");
			return false;
		}
		
		int startPage = i->start >> 8;
		int endPage = i->end >> 8;
		
		for (int j = startPage; j < endPage; j++)
		{
			if (i->read)
				readMap[j] = component;
			if (i->write)
				writeMap[j] = component;
		}
	}
	
	return true;
}

bool MemoryMap16Bit::getMemoryMap(string &range)
{
	range = mappedRange;
	
	return true;
}

int MemoryMap16Bit::read(int address)
{
	return readMap[address >> 8]->read(address);
}

void MemoryMap16Bit::write(int address, int value)
{
	writeMap[address >> 8]->write(address, value);
}

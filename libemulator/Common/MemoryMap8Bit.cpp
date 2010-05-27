
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
		mappedRange.push_back(value);
	else
		return false;
	
	return true;
}

bool MemoryMap8Bit::connect(string name, OEComponent *component)
{
	OEMemoryRange range;
	component->getMemoryMap(range);
	
	setMemoryMap(component, range);
	
	return true;
}

bool MemoryMap8Bit::setMemoryMap(OEComponent *component, OEMemoryRange range)
{
	for (vector<string>::iterator i = range.begin();
		 i != range.end();
		 i++)
	{
		string range = *i;
		size_t separatorPos = range.find('-');
		if (separatorPos == string::npos)
		{
			cerr << "MemoryMap8Bit: range " << range << " invalid.\n";
			return false;
		}
		
		int start = getInt(range.substr(0, separatorPos));
		int end = getInt(range.substr(separatorPos + 1));
		if (start > end)
		{
			cerr << "MemoryMap8Bit: range " << range << " invalid.\n";
			return false;
		}
		
		for (int i = start; i < end; i++)
		{
			readMap[i] = component;
			writeMap[i] = component;
		}
	}
	
	return true;
}

bool MemoryMap8Bit::getMemoryMap(OEMemoryRange &range)
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

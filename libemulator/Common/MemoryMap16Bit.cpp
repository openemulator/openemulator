
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

bool MemoryMap16Bit::setProperty(string name, string value)
{
	if (name == "map")
		mappedRange.push_back(value);
	else
		return false;
	
	return true;
}

bool MemoryMap16Bit::connect(string name, OEComponent *component)
{
	OEMemoryRange range;
	component->getMemoryMap(range);
	
	setMemoryMap(component, range);
	
	return true;
}

bool MemoryMap16Bit::setMemoryMap(OEComponent *component, OEMemoryRange range)
{
	for (vector<string>::iterator i = range.begin();
		 i != range.end();
		 i++)
	{
		string range = *i;
		size_t separatorPos = range.find('-');
		if (separatorPos == string::npos)
		{
			cerr << "MemoryMap16Bit: range " << range << " invalid.\n";
			return false;
		}
		
		int start = getInt(range.substr(0, separatorPos));
		int end = getInt(range.substr(separatorPos + 1));
		if ((start > end) || (start & 0xff) || ((end & 0xff) != 0xff))
		{
			cerr << "MemoryMap16Bit: range " << range << " invalid.\n";
			return false;
		}
		
		int startPage = start >> 8; 
		int endPage = end >> 8; 
		for (int i = startPage; i < endPage; i++)
		{
			readMap[i] = component;
			writeMap[i] = component;
		}
	}
	
	return true;
}

bool MemoryMap16Bit::getMemoryMap(OEMemoryRange &range)
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

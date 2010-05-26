
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

void MemoryMap8Bit::setRange(OEComponent *component, vector<string> mapVector)
{
	for (vector<string>::iterator i = mapVector.begin();
		 i != mapVector.end();
		 i++)
	{
		string range = *i;
		size_t separatorPos = range.find('-');
		if (separatorPos == string::npos)
		{
			cerr << "MemoryMap8Bit: range " << range << " invalid.\n";
			continue;
		}
		
		int start = getInt(range.substr(0, separatorPos));
		int end = getInt(range.substr(separatorPos + 1));
		if (start > end)
		{
			cerr << "MemoryMap8Bit: range " << range << " invalid.\n";
			continue;
		}
		
		for (int i = start; i < end; i++)
		{
			readMap[i] = component;
			writeMap[i] = component;
		}
	}
}

int MemoryMap8Bit::ioctl(int message, void *data)
{
	switch(message)
	{
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "map")
				mappedRange.push_back(property->value);
			
			break;
		}
		case OE_CONNECT:
		{
			OEConnection *connection = (OEConnection *) data;
			OEMemoryMap memoryMap;
			connection->component->ioctl(OE_GET_MEMORYMAP, &memoryMap);
			setRange(memoryMap.component, memoryMap.range);
			
			break;
		}
		case OE_GET_MEMORYMAP:
		{
			OEMemoryMap *memoryMap = (OEMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->range = mappedRange;
			
			break;
		}
		case OE_SET_MEMORYMAP:
		{
			OEMemoryMap *memoryMap = (OEMemoryMap *) data;
			setRange(memoryMap->component, memoryMap->range);
			
			break;
		}
	}
	
	return false;
}

int MemoryMap8Bit::read(int address)
{
	return readMap[address & 0xff]->read(address);
}

void MemoryMap8Bit::write(int address, int value)
{
	readMap[address & 0xff]->write(address, value);
}

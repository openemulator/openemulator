
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

void MemoryMap16Bit::setRange(OEComponent *component, vector<string> mapVector)
{
	for (vector<string>::iterator i = mapVector.begin();
		 i != mapVector.end();
		 i++)
	{
		string range = *i;
		size_t separatorPos = range.find('-');
		if (separatorPos == string::npos)
		{
			cerr << "MemoryMap16Bit: range " << range << " invalid.\n";
			continue;
		}

		int start = getInt(range.substr(0, separatorPos));
		int end = getInt(range.substr(separatorPos + 1));
		if ((start > end) || (start & 0xff) || ((end & 0xff) != 0xff))
		{
			cerr << "MemoryMap16Bit: range " << range << " invalid.\n";
			continue;
		}
		
		int startPage = start >> 8; 
		int endPage = end >> 8; 
		for (int i = startPage; i < endPage; i++)
		{
			readMap[i] = component;
			writeMap[i] = component;
		}
	}
}

int MemoryMap16Bit::ioctl(int message, void *data)
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

int MemoryMap16Bit::read(int address)
{
	return readMap[address >> 8]->read(address);
}

void MemoryMap16Bit::write(int address, int value)
{
	readMap[address >> 8]->write(address, value);
}

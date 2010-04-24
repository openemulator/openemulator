
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
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			OEIoctlMemoryMap memoryMap;
			connection->component->ioctl(OEIOCTL_GET_MEMORYMAP, &memoryMap);
			setRange(memoryMap.component, memoryMap.mapVector);
			
			break;
		}
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "map")
				mapVector.push_back(property->value);
			
			break;
		}
		case OEIOCTL_GET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->mapVector = mapVector;
			
			break;
		}
		case OEIOCTL_SET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			setRange(memoryMap->component, memoryMap->mapVector);
			
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

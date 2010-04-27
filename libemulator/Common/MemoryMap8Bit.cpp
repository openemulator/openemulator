
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
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "map")
				mapVector.push_back(property->value);
			
			break;
		}
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			OEIoctlMemoryMap memoryMap;
			connection->component->ioctl(OEIOCTL_GET_MEMORYMAP, &memoryMap);
			setRange(memoryMap.component, memoryMap.mapVector);
			
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

int MemoryMap8Bit::read(int address)
{
	return readMap[address & 0xff]->read(address);
}

void MemoryMap8Bit::write(int address, int value)
{
	readMap[address & 0xff]->write(address, value);
}

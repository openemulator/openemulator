
/**
 * libemulator
 * Generic memory offset logic
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic memory offset logic
 */

#include "MemoryOffset.h"

MemoryOffset::MemoryOffset()
{
	offset = 0;
}

int MemoryOffset::ioctl(int message, void *data)
{
	switch(message)
	{
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "map")
				mappedRange.push_back(property->value);
			else if (property->name == "offset")
				offset = getInt(property->value);
			
			break;
		}
		case OE_CONNECT:
		{
			OEConnection *connection = (OEConnection *) data;
			if (connection->name == "component")
				component = connection->component;
			
			break;
		}
		case OE_GET_MEMORYMAP:
		{
			OEMemoryMap *memoryMap = (OEMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->range = mappedRange;
			
			break;
		}
	}
	
	return false;
}	

int MemoryOffset::read(int address)
{
	return component->read(address + offset);
}

void MemoryOffset::write(int address, int value)
{
	component->write(address + offset, value);
}

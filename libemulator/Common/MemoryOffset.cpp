
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
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "map")
				mapVector.push_back(property->value);
			else if (property->name == "offset")
				offset = getInt(property->value);
			
			break;
		}
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "component")
				component = connection->component;
			
			break;
		}
		case OEIOCTL_GET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->mapVector = mapVector;
			
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


/**
 * libemulator
 * Generic ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic ROM segment
 */

#include "ROM.h"

ROM::ROM()
{
	mask = 0;
}

int ROM::ioctl(int message, void *data)
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
		case OEIOCTL_SET_RESOURCE:
		{
			OEIoctlData *setData = (OEIoctlData *) data;
			if (setData->name == "image")
			{
				memory = setData->data;
				mask = getNextPowerOf2(memory.size()) - 1;
			}
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
	
	return 0;
}

int ROM::read(int address)
{
	return memory[address & mask];
}

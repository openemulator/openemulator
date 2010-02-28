
/**
 * libemulator
 * Generic ROM
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic ROM segment
 */

#include "ROM.h"

int ROM::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "offset")
				offset = intValue(property->value);
			break;
		}
		case OEIOCTL_SET_RESOURCE:
		{
			OEIoctlData *setData = (OEIoctlData *) data;
			if (setData->name == "image")
				memory = setData->data;
			break;
		}
		case OEIOCTL_GET_MEMORYRANGE:
		{
			OEIoctlMemoryRange *memoryRange = (OEIoctlMemoryRange *) data;
			memoryRange->offset = offset;
			memoryRange->size = memory.size();
			break;
		}
	}
	
	return 0;
}	

int ROM::read(int address)
{
	return memory[address + offset];
}

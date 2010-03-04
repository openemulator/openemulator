
/**
 * libemulator
 * Random generator
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates random output
 */

#include "stdlib.h"

#include "Random.h"

int Random::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "offset")
				offset = intValue(property->value);
			else if (property->name == "size")
				size = intValue(property->value);
			break;
		}
		case OEIOCTL_GET_MEMORYRANGE:
		{
			OEIoctlMemoryRange *memoryRange = (OEIoctlMemoryRange *) data;
			memoryRange->offset = offset;
			memoryRange->size = size;
			break;
		}
	}
	
	return 0;
}	

int Random::read(int address)
{
	return random() & 0xff;
}

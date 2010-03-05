
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
		case OEIOCTL_GET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->offset = 0;
			memoryMap->size = INT_MAX;
			break;
		}
	}
	
	return 0;
}	

int Random::read(int address)
{
	return random() & 0xff;
}

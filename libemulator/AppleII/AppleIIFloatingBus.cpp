
/**
 * libemulator
 * Apple II Floating Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Provides the Apple II floating bus
 */

#include "stdlib.h"

#include "AppleIIFloatingBus.h"

int AppleIIFloatingBus::ioctl(int message, void *data)
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

int AppleIIFloatingBus::read(int address)
{
	return 0;
}

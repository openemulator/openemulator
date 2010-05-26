
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
		case OE_GET_MEMORYMAP:
		{
			OEMemoryMap *memoryMap = (OEMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->range.push_back("0x0-0xffffffff");
			break;
		}
	}
	
	return 0;
}

int AppleIIFloatingBus::read(int address)
{
	return 0;
}

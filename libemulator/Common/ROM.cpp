
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
	
	memory.resize(1);
}

int ROM::ioctl(int message, void *data)
{
	switch(message)
	{
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "map")
				mappedRange.push_back(property->value);
			
			break;
		}
		case OE_SET_RESOURCE:
		{
			OEData *setData = (OEData *) data;
			if (setData->name == "image")
			{
				memory = setData->data;
				int size = getPreviousPowerOf2(memory.size());
				if (size < 1)
					size = 1;
				memory.resize(size);
				mask = size - 1;
			}
			
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
	
	return 0;
}

int ROM::read(int address)
{
	return memory[address & mask];
}

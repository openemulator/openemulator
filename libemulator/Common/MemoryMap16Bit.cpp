
/**
 * libemulator
 * 16 bit memory map
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic 16-bit memory map
 */

#include "MemoryMap16Bit.h"

MemoryMap16Bit::MemoryMap16Bit()
{
	for (int i = 0; i < MEMORYMAP16BIT_SIZE; i++)
		readMap[i] = writeMap[i] = 0;
}

void MemoryMap16Bit::setRange(OEComponent *component, int start, int end)
{
	start += MEMORYMAP16BIT_PAGE - 1;
	start >>= MEMORYMAP16BIT_SHIFT;
	end -= MEMORYMAP16BIT_PAGE - 1;
	end >>= MEMORYMAP16BIT_SHIFT;
	for (int i = start; i < end; i++)
	{
		readMap[i] = component;
		writeMap[i] = component;
	}
}

int MemoryMap16Bit::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			OEIoctlMemoryMap memoryMap;
			connection->component->ioctl(OEIOCTL_GET_MEMORYMAP, &memoryMap);
			setRange(memoryMap.component, memoryMap.offset, memoryMap.size);
			break;
		}
		case OEIOCTL_GET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->offset = 0;
			memoryMap->size = MEMORYMAP16BIT_SIZE;
			break;
		}
		case OEIOCTL_SET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			setRange(memoryMap->component, memoryMap->offset, memoryMap->size);
			break;
		}
	}
	
	return 0;
}

int MemoryMap16Bit::read(int address)
{
	return readMap[address >> MEMORYMAP16BIT_SHIFT]->read(address);
}

void MemoryMap16Bit::write(int address, int value)
{
	readMap[address >> MEMORYMAP16BIT_SHIFT]->write(address, value);
}

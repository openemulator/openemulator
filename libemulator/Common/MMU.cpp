
/**
 * libemulator
 * MMU
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a generic memory management unit
 */

#include "MMU.h"

MMU::MMU()
{
	for (int i = 0; i < MMU_MAPSIZE; i++)
		readMap[i] = writeMap[i] = 0;
}

void MMU::setFloatingBus(OEComponent *component)
{
	for (int i = 0; i < MMU_MAPSIZE; i++)
	{
		if (!readMap[i])
			readMap[i] = component;
		if (!writeMap[i])
			writeMap[i] = component;
	}
}

void MMU::setRange(OEComponent *component, int start, int end)
{
	start >>= MMU_PAGESHIFT;
	end >>= MMU_PAGESHIFT;
	for (int i = start; i < end; i++)
	{
		readMap[i] = component;
		writeMap[i] = component;
	}
}

int MMU::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "floatingBus")
				setFloatingBus(connection->component);
			else
			{
				OEIoctlMemoryRange memoryRange;
				connection->component->ioctl(OEIOCTL_GET_MEMORYRANGE, &memoryRange);
				setRange(connection->component, memoryRange.offset, memoryRange.size);
			}
			break;
		}
		case OEIOCTL_GET_MEMORYRANGE:
		{
			OEIoctlMemoryRange *memoryRange = (OEIoctlMemoryRange *) data;
			memoryRange->offset = 0;
			memoryRange->size = MMU_MAPSIZE;
			break;
		}
	}
	
	return 0;
}

int MMU::read(int address)
{
	return readMap[address >> MMU_PAGESHIFT]->read(address);
}

void MMU::write(int address, int value)
{
	readMap[address >> MMU_PAGESHIFT]->write(address, value);
}

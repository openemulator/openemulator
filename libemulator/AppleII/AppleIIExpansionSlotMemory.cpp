
/**
 * libemulator
 * Apple II Expanded Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C800-CFFF range.
 */

#include "AppleIIExpansionSlotMemory.h"

int AppleIIExpansionSlotMemory::ioctl(int message, void *data)
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
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "floatingBus")
				floatingBus = connection->component;
			break;
		}
		case OEIOCTL_GET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->offset = offset;
			memoryMap->size = APPLEIIEXPANSIONSLOTMEMORY_SIZE;
			break;
		}
		case APPLEIIEXPANSIONSLOTMEMORY_SET_SLOT:
		{
			expandedSlotMemory = (OEComponent *) data;
			break;
		}
	}
	
	return 0;
}

int AppleIIExpansionSlotMemory::read(int address)
{
	if ((address & APPLEIIEXPANSIONSLOTMEMORY_MASK) == APPLEIIEXPANSIONSLOTMEMORY_MASK)
		expandedSlotMemory = floatingBus;
	
	return expandedSlotMemory->read(address);
}

void AppleIIExpansionSlotMemory::write(int address, int value)
{
	if ((address & (APPLEIIEXPANSIONSLOTMEMORY_MASK)) == APPLEIIEXPANSIONSLOTMEMORY_MASK)
		expandedSlotMemory = floatingBus;
	
	expandedSlotMemory->write(address, value);
}

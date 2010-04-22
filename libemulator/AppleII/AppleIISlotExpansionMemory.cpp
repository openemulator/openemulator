
/**
 * libemulator
 * Apple II Slot Expansion Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C800-CFFF range.
 */

#include "AppleIISlotExpansionMemory.h"

int AppleIISlotExpansionMemory::ioctl(int message, void *data)
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
			memoryMap->mapVector = mapVector;
			break;
		}
		case APPLEIISLOTEXPANSIONMEMORY_SET_SLOT:
		{
			expandedSlotMemory = (OEComponent *) data;
			break;
		}
	}
	
	return 0;
}

int AppleIISlotExpansionMemory::read(int address)
{
	if ((address & APPLEIISLOTEXPANSIONMEMORY_MASK) ==
		APPLEIISLOTEXPANSIONMEMORY_MASK)
		expandedSlotMemory = floatingBus;
	
	return expandedSlotMemory->read(address);
}

void AppleIISlotExpansionMemory::write(int address, int value)
{
	if ((address & (APPLEIISLOTEXPANSIONMEMORY_MASK)) ==
		APPLEIISLOTEXPANSIONMEMORY_MASK)
		expandedSlotMemory = floatingBus;
	
	expandedSlotMemory->write(address, value);
}


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
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "map")
				mappedRange.push_back(property->value);
			break;
		}
		case OE_CONNECT:
		{
			OEConnection *connection = (OEConnection *) data;
			if (connection->name == "floatingBus")
				floatingBus = connection->component;
			break;
		}
		case OE_GET_MEMORYMAP:
		{
			OEMemoryMap *memoryMap = (OEMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->range = mappedRange;
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

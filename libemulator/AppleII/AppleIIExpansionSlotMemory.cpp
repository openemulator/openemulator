
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
		case OEIOCTL_CONNECT:
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "floatingBus")
				floatingBus = connection->component;
			else if (connection->name == "expandedSlotMemory")
				expandedSlotMemory = connection->component;
	}
	
	return 0;
}

int AppleIIExpansionSlotMemory::read(int address)
{
	if (address == 0xcfff)
		expandedSlotMemory = floatingBus;
	
	return expandedSlotMemory->read(address);
}

void AppleIIExpansionSlotMemory::write(int address, int value)
{
	if (address == 0xcfff)
		expandedSlotMemory = floatingBus;
	
	expandedSlotMemory->write(address, value);
}

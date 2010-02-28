
/**
 * libemulator
 * Apple II Expanded Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C800-CFFF range.
 */

#include "AppleIIExpandedSlotMemory.h"

int AppleIIExpandedSlotMemory::ioctl(int message, void *data)
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

int AppleIIExpandedSlotMemory::read(int address)
{
	if (address == 0xcfff)
		expandedSlotMemory = floatingBus;
	
	return expandedSlotMemory->read(address);
}

void AppleIIExpandedSlotMemory::write(int address, int value)
{
	if (address == 0xcfff)
		expandedSlotMemory = floatingBus;
	
	expandedSlotMemory->write(address, value);
}

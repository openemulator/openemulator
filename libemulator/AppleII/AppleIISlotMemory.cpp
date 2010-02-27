
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C100-C7FF range.
 */

#include "AppleIISlotMemory.h"

int AppleIISlotMemory::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
			OEIoctlConnection *conn = (OEIoctlConnection *) data;
			if (conn->name == "floatingBus")
				floatingBus = conn->component;
			else if (conn->name == "expandedSlotMemory")
				expandedSlotMemory = conn->component;
	}
	
	return 0;
}

int AppleIISlotMemory::read(int address)
{
	int index = (address >> 12) & 0x7;
	connectionMessage.component = slotMemory[index];
	expandedSlotMemory->ioctl(OEIOCTL_CONNECT, &connectionMessage);
	return slotMemory[index]->read(address);
}

void AppleIISlotMemory::write(int address, int value)
{
	int index = (address >> 12) & 0x7;
	connectionMessage.component = slotMemory[index];
	expandedSlotMemory->ioctl(OEIOCTL_CONNECT, &connectionMessage);
	expandedSlotMemory->write((address >> 12) & 0x7, value);
}

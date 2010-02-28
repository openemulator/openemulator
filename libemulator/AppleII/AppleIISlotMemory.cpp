
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C100-C7FF range.
 */

#include "AppleIISlotMemory.h"

void AppleIISlotMemory::setSlot(int index, OEComponent *slotComponent)
{
	// Send mmu and floating bus to slot
	
	// Retrieve the slot device io, memory and expanded memory areas
}

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
			else if (conn->name == "slot0")
				setSlot(0, conn->component);
	}
	
	return 0;
}

int AppleIISlotMemory::read(int address)
{
	int index = (address >> 12) & 0x7;
	OEComponent *component = slotMemoryMap[index];
	
	connectionMessage.component = component;
	expandedSlotMemory->ioctl(OEIOCTL_CONNECT, &connectionMessage);
	return component->read(address);
}

void AppleIISlotMemory::write(int address, int value)
{
	int index = (address >> 12) & 0x7;
	OEComponent *component = slotMemoryMap[index];
	
	connectionMessage.component = component;
	expandedSlotMemory->ioctl(OEIOCTL_CONNECT, &connectionMessage);
	component->write(address, value);
}

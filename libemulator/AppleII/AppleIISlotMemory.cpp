
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C100-C7FF range.
 */

#include "AppleIISlotMemory.h"
#include "MemoryMap8bit.h"

void AppleIISlotMemory::setSlot(int index, OEComponent *component)
{
	OEIoctlConnection connection;
	
	connection.name = "system";
	connection.component = system;
	component->ioctl(OEIOCTL_CONNECT, &connection);
	
	connection.name = "mmu";
	connection.component = mmu;
	component->ioctl(OEIOCTL_CONNECT, &connection);
	
	connection.name = "floatingBus";
	connection.component = floatingBus;
	component->ioctl(OEIOCTL_CONNECT, &connection);
	
	AppleIISlotMemoryQuery slotMemoryQuery;
	component->ioctl(APPLEIISLOTMEMORY_QUERY, &slotMemoryQuery);
	
	OEIoctlMapMemoryRange mapMemoryRange;
	mapMemoryRange.component = component;
	mapMemoryRange.offset = 0x80 + (index << 4);
	mapMemoryRange.size = 0x10;
	io->ioctl(OEIOCTL_MAP_MEMORYRANGE, &mapMemoryRange);
	slotMemoryMap[index] = slotMemoryQuery.slotMemory;
	expandedSlotMemoryMap[index] = slotMemoryQuery.expandedSlotMemory;
}

int AppleIISlotMemory::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "system")
				system = connection->component;
			else if (connection->name == "mmu")
				mmu = connection->component;
			else if (connection->name == "floatingBus")
				floatingBus = connection->component;
			else if (connection->name == "io")
				io = connection->component;
			else if (connection->name == "expandedSlotMemory")
				expandedSlotMemory = connection->component;
			else if (connection->name == "slot0")
				setSlot(0, connection->component);
			else if (connection->name == "slot1")
				setSlot(1, connection->component);
			else if (connection->name == "slot2")
				setSlot(2, connection->component);
			else if (connection->name == "slot3")
				setSlot(3, connection->component);
			else if (connection->name == "slot4")
				setSlot(4, connection->component);
			else if (connection->name == "slot5")
				setSlot(5, connection->component);
			else if (connection->name == "slot6")
				setSlot(6, connection->component);
			else if (connection->name == "slot7")
				setSlot(7, connection->component);
			break;
		}
		case OEIOCTL_SET_PROPERTY:
		{
			OEIoctlProperty *property = (OEIoctlProperty *) data;
			if (property->name == "offset")
				offset = intValue(property->value);
			break;
		}
		case OEIOCTL_GET_MEMORYRANGE:
		{
			OEIoctlMemoryRange *memoryRange = (OEIoctlMemoryRange *) data;
			memoryRange->offset = offset;
			memoryRange->size = APPLEIISLOTMEMORY_SIZE;
			break;
		}
	}
	
	return 0;
}

int AppleIISlotMemory::read(int address)
{
	OEComponent *component = slotMemoryMap[(address >> 12) & 0x7];
	
	OEIoctlConnection connection;
	connection.name = "expandedSlotMemory";
	connection.component = component;
	expandedSlotMemory->ioctl(OEIOCTL_CONNECT, &connection);
	
	return component->read(address);
}

void AppleIISlotMemory::write(int address, int value)
{
	OEComponent *component = slotMemoryMap[(address >> 12) & 0x7];
	
	OEIoctlConnection connection;
	connection.name = "expandedSlotMemory";
	connection.component = component;
	expandedSlotMemory->ioctl(OEIOCTL_CONNECT, &connection);
	
	component->write(address, value);
}

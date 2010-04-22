
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple II C100-C7FF range.
 */

#include <sstream>

#include "AppleIISlotMemory.h"

#include "AppleIISlotExpansionMemory.h"
#include "MemoryMap8bit.h"

void AppleIISlotMemory::setSlot(int index, OEComponent *component)
{
	OEIoctlConnection connection;
	
	connection.name = "system";
	connection.component = system;
	component->ioctl(OEIOCTL_CONNECT, &connection);
	
	connection.name = "memoryMap";
	connection.component = floatingBus;
	component->ioctl(OEIOCTL_CONNECT, &connection);
	
	connection.name = "mmu";
	connection.component = mmu;
	component->ioctl(OEIOCTL_CONNECT, &connection);
	
	connection.name = "floatingBus";
	connection.component = mmu;
	component->ioctl(OEIOCTL_CONNECT, &connection);
	
	AppleIISlotMemoryQuery slotMemoryQuery;
	component->ioctl(APPLEIISLOTMEMORY_QUERY, &slotMemoryQuery);
	
	stringstream ss;
	int baseAddr = 0xc080 + index << 4;
	ss << std::hex << baseAddr << "-" << (baseAddr + 0xf);
	
	OEIoctlMemoryMap memoryMap;
	memoryMap.component = component;
	memoryMap.mapVector.push_back(ss.str());
	
	ioMap->ioctl(OEIOCTL_SET_MEMORYMAP, &memoryMap);
	slotMemoryMap[index] = slotMemoryQuery.slotMemory;
	slotExpansionMemoryMap[index] = slotMemoryQuery.slotExpansionMemory;
}

int AppleIISlotMemory::ioctl(int message, void *data)
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
			if (connection->name == "system")
				system = connection->component;
			else if (connection->name == "memoryMap")
				memoryMap = connection->component;
			else if (connection->name == "mmu")
				mmu = connection->component;
			else if (connection->name == "floatingBus")
				floatingBus = connection->component;
			else if (connection->name == "ioMap")
				ioMap = connection->component;
			else if (connection->name == "slotExpansionMemory")
				slotExpansionMemory = connection->component;
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
		case OEIOCTL_GET_MEMORYMAP:
		{
			OEIoctlMemoryMap *memoryMap = (OEIoctlMemoryMap *) data;
			memoryMap->component = this;
			memoryMap->mapVector = mapVector;
			break;
		}
	}
	
	return 0;
}

int AppleIISlotMemory::read(int address)
{
	OEComponent *component = slotMemoryMap[(address >> 12) & 0x7];
	slotExpansionMemory->ioctl(APPLEIISLOTEXPANSIONMEMORY_SET_SLOT, &component);
	return component->read(address);
}

void AppleIISlotMemory::write(int address, int value)
{
	OEComponent *component = slotMemoryMap[(address >> 12) & 0x7];
	slotExpansionMemory->ioctl(APPLEIISLOTEXPANSIONMEMORY_SET_SLOT, &component);
	component->write(address, value);
}


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
	component->connect("system", system);
	component->connect("memoryMap", memoryMap);
	component->connect("mmu", mmu);
	component->connect("floatingBus", floatingBus);
	
	AppleIISlotMemoryQuery slotMemoryQuery;
	component->ioctl(APPLEIISLOTMEMORY_QUERY, &slotMemoryQuery);
	
	int start = 0xc080 + index << 4;
	int end = start + 0x000f;
	
	OEMemoryRange range;
	range.push_back(getRange(start, end));
	ioMap->setMemoryMap(this, range);
	
	slotMemoryMap[index] = slotMemoryQuery.slotMemory;
	slotExpansionMemoryMap[index] = slotMemoryQuery.slotExpansionMemory;
}

bool AppleIISlotMemory::setProperty(string name, string &value)
{
	if (name == "map")
		mappedRange.push_back(value);
	else
		return false;
	
	return true;
}

bool AppleIISlotMemory::connect(string name, OEComponent *component)
{
	if (name == "system")
		system = component;
	else if (name == "memoryMap")
		memoryMap = component;
	else if (name == "mmu")
		mmu = component;
	else if (name == "floatingBus")
		floatingBus = component;
	else if (name == "ioMap")
		ioMap = component;
	else if (name == "slotExpansionMemory")
		slotExpansionMemory = component;
	else if (name == "slot0")
		setSlot(0, component);
	else if (name == "slot1")
		setSlot(1, component);
	else if (name == "slot2")
		setSlot(2, component);
	else if (name == "slot3")
		setSlot(3, component);
	else if (name == "slot4")
		setSlot(4, component);
	else if (name == "slot5")
		setSlot(5, component);
	else if (name == "slot6")
		setSlot(6, component);
	else if (name == "slot7")
		setSlot(7, component);
	else
		return false;
	
	return true;
}

bool AppleIISlotMemory::getMemoryMap(OEMemoryRange &range)
{
	range = mappedRange;
	
	return true;
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

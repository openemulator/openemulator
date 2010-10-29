
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II's slot memory range ($C100-$C7FF).
 */

#include "AppleIISlotMemory.h"
#include "AppleIISlotExpansionMemory.h"

bool AppleIISlotMemory::setValue(string name, string value)
{
	if (name == "slotSel")
		slotSel = getInt(value);
	else
		return false;
	
	return true;
}

bool AppleIISlotMemory::setComponent(string name, OEComponent *component)
{
	if (name == "floatingBus")
		floatingBus = component;
	else if (name == "slotExpansionMemory")
		slotExpansionMemory = component;
	else if (name == "slot1")
		slot[3] = component;
	else if (name == "slot2")
		slot[3] = component;
	else if (name == "slot3")
		slot[3] = component;
	else if (name == "slot4")
		slot[4] = component;
	else if (name == "slot5")
		slot[5] = component;
	else if (name == "slot6")
		slot[6] = component;
	else if (name == "slot7")
		slot[7] = component;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIISlotMemory::read(OEAddress address)
{
	OEComponent *component = slot[(address >> 12) & 0x7];
	slotExpansionMemory->postMessage(this, APPLEIISLOTEXPANSIONMEMORY_SET_SLOT, component);
	return component->read(address);
}

void AppleIISlotMemory::write(OEAddress address, OEUInt8 value)
{
	OEComponent *component = slot[(address >> 12) & 0x7];
	slotExpansionMemory->postMessage(this, APPLEIISLOTEXPANSIONMEMORY_SET_SLOT, component);
	component->write(address, value);
}

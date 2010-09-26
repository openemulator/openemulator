
/**
 * libemulator
 * Apple II Slot Expansion Memory
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II's slot expansion memory range ($C800-$CFFF).
 */

#include "AppleIISlotExpansionMemory.h"

#define APPLEIISLOTEXPANSIONMEMORY_MASK	0x7ff

bool AppleIISlotExpansionMemory::setComponent(string name, OEComponent *component)
{
	if (name == "floatingBus")
		floatingBus = component;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIISlotExpansionMemory::read(OEAddress address)
{
	if (!((~address) & APPLEIISLOTEXPANSIONMEMORY_MASK))
		slot = floatingBus;
	
	return slot->read(address);
}

void AppleIISlotExpansionMemory::write(OEAddress address, OEUInt8 value)
{
	if (!((~address) & APPLEIISLOTEXPANSIONMEMORY_MASK))
		slot = floatingBus;
	
	slot->write(address, value);
}


/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot memory ($C100-$C7FF)
 */

#include "AppleIISlotMemory.h"

#include "AppleIIInterface.h"

bool AppleIISlotMemory::setValue(string name, string value)
{
	if (name == "slotSel")
		slotSel = (OEUInt32) getUInt(value);
	else
		return false;
	
    for (OEUInt32 i = 0; i < 8; i++)
        slot[i] = 0;
    
	return true;
}

bool AppleIISlotMemory::setRef(string name, OEComponent *ref)
{
	if (name == "floatingBus")
		floatingBus = ref;
	else if (name == "slotExpansionMemory")
		slotExpansionMemory = ref;
	else if (name == "slot1")
		slot[1] = ref;
	else if (name == "slot2")
		slot[2] = ref;
	else if (name == "slot3")
		slot[3] = ref;
	else if (name == "slot4")
		slot[4] = ref;
	else if (name == "slot5")
		slot[5] = ref;
	else if (name == "slot6")
		slot[6] = ref;
	else if (name == "slot7")
		slot[7] = ref;
	else
		return false;
	
	return true;
}

bool init()
{
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

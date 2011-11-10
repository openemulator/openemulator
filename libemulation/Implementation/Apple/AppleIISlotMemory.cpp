
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
        setSlot(1, ref);
	else if (name == "slot2")
        setSlot(2, ref);
	else if (name == "slot3")
        setSlot(3, ref);
	else if (name == "slot4")
        setSlot(4, ref);
	else if (name == "slot5")
        setSlot(5, ref);
	else if (name == "slot6")
        setSlot(6, ref);
	else if (name == "slot7")
        setSlot(7, ref);
	else
		return false;
	
	return true;
}

bool AppleIISlotMemory::init()
{
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    if (!slotExpansionMemory)
    {
        logMessage("slotExpansionMemory not connected");
        
        return false;
    }
    
	OEComponent *component = slot[slotSel];
    
//	slotExpansionMemory->postMessage(this, APPLEIISLOTMEMORY_RESET, component);
	
    return true;
}

OEUInt8 AppleIISlotMemory::read(OEAddress address)
{
    slotSel = (address >> 12) & 0x7;
    
	OEComponent *component = slot[slotSel];
    
//	slotExpansionMemory->postMessage(this, APPLEIISLOTMEMORY_RESET, component);
	
    return component->read(address);
}

void AppleIISlotMemory::write(OEAddress address, OEUInt8 value)
{
    slotSel = (address >> 12) & 0x7;
    
	OEComponent *component = slot[slotSel];
    
//	slotExpansionMemory->postMessage(this, APPLEIISLOTMEMORY_RESET, component);
	
    component->write(address, value);
}

void AppleIISlotMemory::setSlot(OEUInt32 slotIndex, OEComponent *ref)
{
    slot[slotIndex] = ref ? ref : floatingBus;
}


/**
 * libemulator
 * Apple II Slot Expansion Memory
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot expansion memory ($C800-$CFFF)
 */

#include "AppleIISlotExpansionMemory.h"

#define APPLEIISLOTEXPANSIONMEMORY_MASK	0x7ff

AppleIISlotExpansionMemory::AppleIISlotExpansionMemory()
{
    floatingBus = NULL;
}

bool AppleIISlotExpansionMemory::setRef(string name, OEComponent *ref)
{
	if (name == "floatingBus")
		floatingBus = ref;
	else
		return false;
	
	return true;
}

bool AppleIISlotExpansionMemory::init()
{
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    return true;
}

OEUInt8 AppleIISlotExpansionMemory::read(OEAddress address)
{
	if (!((~address) & APPLEIISLOTEXPANSIONMEMORY_MASK))
		slotExpansionMemory = floatingBus;
	
	return slotExpansionMemory->read(address);
}

void AppleIISlotExpansionMemory::write(OEAddress address, OEUInt8 value)
{
	if (!((~address) & APPLEIISLOTEXPANSIONMEMORY_MASK))
		slotExpansionMemory = floatingBus;
	
	slotExpansionMemory->write(address, value);
}

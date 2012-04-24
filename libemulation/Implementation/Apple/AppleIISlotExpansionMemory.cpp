
/**
 * libemulator
 * Apple II Slot Expansion Memory
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot expansion memory ($C800-$CFFF)
 */

#include "AppleIISlotExpansionMemory.h"

#include "AppleIIInterface.h"

AppleIISlotExpansionMemory::AppleIISlotExpansionMemory()
{
    mmu = NULL;
    memory = NULL;
}

bool AppleIISlotExpansionMemory::setRef(string name, OEComponent *ref)
{
	if (name == "mmu")
		mmu = ref;
	else if (name == "memory")
		memory = ref;
	else
		return false;
	
	return true;
}

bool AppleIISlotExpansionMemory::init()
{
    if (!mmu)
    {
        logMessage("mmu not connected");
        
        return false;
    }
    
    if (!memory)
    {
        logMessage("memory not connected");
        
        return false;
    }
    
    return true;
}

OEChar AppleIISlotExpansionMemory::read(OEAddress address)
{
    if (address == 0xcfff)
        mmu->postNotification(this, APPLEII_SLOTEXPANSIONMEMORY_WILL_UNMAP, NULL);
	
	return memory->read(address);
}

void AppleIISlotExpansionMemory::write(OEAddress address, OEChar value)
{
    if (address == 0xcfff)
        mmu->postNotification(this, APPLEII_SLOTEXPANSIONMEMORY_WILL_UNMAP, NULL);
    
	memory->write(address, value);
}

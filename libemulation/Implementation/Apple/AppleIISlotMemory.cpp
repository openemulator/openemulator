
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

AppleIISlotMemory::AppleIISlotMemory()
{
    en = false;
    
    slotMemory = NULL;
    slotExpansionMemory = NULL;
}

bool AppleIISlotMemory::setValue(string name, string value)
{
	if (name == "en")
		en = (OEUInt32) getUInt(value);
	else
		return false;
    
	return true;
}

bool AppleIISlotMemory::getValue(string name, string &value)
{
	if (name == "en")
        value = getString(en);
	else
		return false;
    
    return true;
}

bool AppleIISlotMemory::setRef(string name, OEComponent *ref)
{
	if (name == "mmu")
    {
        if (mmu)
            mmu->removeObserver(this, APPLEIIMMU_SLOTEXPANSIONMEMORY_WILL_UNMAP);
		mmu = ref;
        if (mmu)
            mmu->addObserver(this, APPLEIIMMU_SLOTEXPANSIONMEMORY_WILL_UNMAP);
    }
	else if (name == "slotMemory")
		slotMemory = ref;
	else if (name == "slotExpansionMemory")
		slotExpansionMemory = ref;
	else
		return false;
	
	return true;
}

bool AppleIISlotMemory::init()
{
    if (!slotMemory)
    {
        logMessage("slotMemory not connected");
        
        return false;
    }
    
    if (en)
        mmu->postMessage(this, APPLEIIMMU_MAP_SLOTEXPANSION, &slotExpansionMemory);
    
    return true;
}

void AppleIISlotMemory::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == mmu)
    {
        en = false;
        
        mmu->postMessage(this, APPLEIIMMU_UNMAP_SLOTEXPANSION, &slotExpansionMemory);
    }
}

OEUInt8 AppleIISlotMemory::read(OEAddress address)
{
    if (!en)
    {
        en = true;
        
        mmu->postMessage(this, APPLEIIMMU_MAP_SLOTEXPANSION, &slotExpansionMemory);
    }
    
    return slotMemory->read(address);
}

void AppleIISlotMemory::write(OEAddress address, OEUInt8 value)
{
    if (!en)
    {
        en = true;
        
        mmu->postMessage(this, APPLEIIMMU_MAP_SLOTEXPANSION, &slotExpansionMemory);
    }
    
    slotMemory->write(address, value);
}

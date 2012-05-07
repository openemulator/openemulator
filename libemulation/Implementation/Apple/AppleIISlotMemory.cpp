
/**
 * libemulator
 * Apple II Slot Memory
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II slot memory ($C100-$C7FF)
 */

#include "AppleIISlotMemory.h"

#include "MemoryInterface.h"

#include "AppleIIInterface.h"

AppleIISlotMemory::AppleIISlotMemory()
{
    memoryBus = NULL;
    memory = NULL;
    en = false;
}

bool AppleIISlotMemory::setValue(string name, string value)
{
	if (name == "en")
		en = getOEInt(value);
	else
		return false;
    
	return true;
}

bool AppleIISlotMemory::getValue(string name, string& value)
{
	if (name == "en")
        value = getString(en);
	else
		return false;
    
    return true;
}

bool AppleIISlotMemory::setRef(string name, OEComponent *ref)
{
	if (name == "memoryBus")
    {
        if (memoryBus)
            memoryBus->removeObserver(this, APPLEII_SLOTEXPANSIONMEMORY_WILL_UNMAP);
		memoryBus = ref;
        if (memoryBus)
            memoryBus->addObserver(this, APPLEII_SLOTEXPANSIONMEMORY_WILL_UNMAP);
    }
	else if (name == "memory")
		memory = ref;
	else
		return false;
	
	return true;
}

bool AppleIISlotMemory::init()
{
    if (!memoryBus)
    {
        logMessage("memoryBus not connected");
        
        return false;
    }
    
    if (!memory)
    {
        logMessage("memory not connected");
        
        return false;
    }
    
    if (en)
        updateSlotExpansion(true);
    
    return true;
}

void AppleIISlotMemory::dispose()
{
    if (en)
        updateSlotExpansion(false);
}

void AppleIISlotMemory::notify(OEComponent *sender, int notification, void *data)
{
    enableSlotExpansion(false);
}

OEChar AppleIISlotMemory::read(OEAddress address)
{
    if (!(address & 0x800))
        enableSlotExpansion(true);
    else if (!((~address) & 0xfff))
        memoryBus->postNotification(this, APPLEII_SLOTEXPANSIONMEMORY_WILL_UNMAP, NULL);
    
    return memory->read(address);
}

void AppleIISlotMemory::write(OEAddress address, OEChar value)
{
    if (!(address & 0x800))
        enableSlotExpansion(true);
    else if (!((~address) & 0xfff))
        memoryBus->postNotification(this, APPLEII_SLOTEXPANSIONMEMORY_WILL_UNMAP, NULL);
    
    memory->write(address, value);
}

void AppleIISlotMemory::enableSlotExpansion(bool value)
{
    if (en == value)
        return;
    
    updateSlotExpansion(value);
}

void AppleIISlotMemory::updateSlotExpansion(bool value)
{
    en = value;
    
    MemoryMaps memoryMaps;
    MemoryMap memoryMap;
    
    memoryMap.component = this;
    memoryMap.startAddress = 0xc800;
    memoryMap.endAddress = 0xcfff;
    memoryMap.read = true;
    memoryMap.write = true;
    memoryMaps.push_back(memoryMap);
    
    memoryBus->postMessage(this, (value ?
                                  APPLEII_MAP_SLOTMEMORYMAPS :
                                  APPLEII_UNMAP_SLOTMEMORYMAPS), &memoryMaps);
}

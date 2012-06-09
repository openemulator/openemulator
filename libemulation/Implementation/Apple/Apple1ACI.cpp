
/**
 * libemulation
 * Apple-1 ACI
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 ACI (Apple Cassette Interface)
 */

#include "Apple1ACI.h"

#include "MemoryInterface.h"

Apple1ACI::Apple1ACI() : Audio1Bit()
{
    memoryBus = NULL;
    rom = NULL;
}

bool Apple1ACI::setRef(string name, OEComponent *ref)
{
    if (name == "memoryBus")
        memoryBus = ref;
    else if (name == "rom")
        rom = ref;
    else
        return Audio1Bit::setRef(name, ref);
    
    return true;
}

bool Apple1ACI::init()
{
    if (!Audio1Bit::init())
        return false;
    
    if (!memoryBus)
    {
        logMessage("memoryBus not connected");
        
        return false;
    }
    
    if (!rom)
    {
        logMessage("rom not connected");
        
        return false;
    }
    
    updateMemory(true);
    
    return true;
}

void Apple1ACI::dispose()
{
    updateMemory(false);
}

OEChar Apple1ACI::read(OEAddress address)
{
    if (address & 0x80)
    {
        if (readAudioInput())
            address &= ~0x1;
    }
    
    toggleAudioOutput();
    
    return rom->read(address);
}

void Apple1ACI::write(OEAddress address, OEChar value)
{
    toggleAudioOutput();
}

void Apple1ACI::updateMemory(bool value)
{
    if (!memoryBus)
        return;
    
    OEInt message = value ? ADDRESSDECODER_MAP : ADDRESSDECODER_UNMAP;
    
    MemoryMap memoryMap;
    
    memoryMap.component = this;
    memoryMap.startAddress = 0xc000;
    memoryMap.endAddress = 0xc0ff;
    memoryMap.read = true;
    memoryMap.write = true;
    memoryBus->postMessage(this, message, &memoryMap);
    
    memoryMap.component = rom;
    memoryMap.startAddress = 0xc100;
    memoryMap.endAddress = 0xc1ff;
    memoryMap.read = true;
    memoryMap.write = false;
    memoryBus->postMessage(this, message, &memoryMap);
}

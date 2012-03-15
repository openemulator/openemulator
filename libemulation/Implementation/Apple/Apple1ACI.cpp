
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
    
    mapMemory(ADDRESSDECODER_MAP_MEMORYMAPS);
    
    return true;
}

void Apple1ACI::dispose()
{
    mapMemory(ADDRESSDECODER_UNMAP_MEMORYMAPS);
}

OEUInt8 Apple1ACI::read(OEAddress address)
{
    if (address & 0x80)
    {
        if (readAudioInput())
            address &= ~0x1;
    }
    
    toggleAudioOutput();
    
    return rom->read(address);
}

void Apple1ACI::write(OEAddress address, OEUInt8 value)
{
    toggleAudioOutput();
}

void Apple1ACI::mapMemory(int message)
{
    MemoryMaps m;
    MemoryMap theMap;
    
    theMap.component = this;
    theMap.startAddress = 0xc000;
    theMap.endAddress = 0xc0ff;
    theMap.read = true;
    theMap.write = true;
    m.push_back(theMap);
    
    theMap.component = rom;
    theMap.startAddress = 0xc100;
    theMap.endAddress = 0xc1ff;
    theMap.read = true;
    theMap.write = false;
    m.push_back(theMap);
    
    if (memoryBus)
        memoryBus->postMessage(this, message, &m);
}

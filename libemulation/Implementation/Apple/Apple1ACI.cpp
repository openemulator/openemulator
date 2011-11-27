
/**
 * libemulation
 * Apple-1 ACI
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 ACI (Apple Cassette Interface)
 */

#include "Apple1ACI.h"

#include "MemoryInterface.h"

Apple1ACI::Apple1ACI()
{
    noiseRejection = 5;
    
    rom = NULL;
    mmu = NULL;
    audioCodec = NULL;
    
    audioLevel = 0x80;
    threshold = 0x80;
}

bool Apple1ACI::setValue(string name, string value)
{
    if (name == "noiseRejection")
        noiseRejection = getInt(value);
    else
        return false;
    
    return true;
}

bool Apple1ACI::getValue(string name, string &value)
{
    if (name == "noiseRejection")
        value = getString(noiseRejection);
    else
        return false;
    
    return true;
}

bool Apple1ACI::setRef(string name, OEComponent *ref)
{
    if (name == "rom")
        rom = ref;
    else if (name == "mmu")
        mmu = ref;
    else if (name == "audioCodec")
        audioCodec = ref;
    else
        return false;
    
    return true;
}

bool Apple1ACI::init()
{
    if (!rom)
    {
        logMessage("rom not connected");
        
        return false;
    }
    
    if (!mmu)
    {
        logMessage("mmu not connected");
        
        return false;
    }
    
    if (!audioCodec)
    {
        logMessage("audioCodec not connected");
        
        return false;
    }
    
    mapMMU(MMU_MAP);
    
    return true;
}

void Apple1ACI::dispose()
{
    mapMMU(MMU_UNMAP);
}

OEUInt8 Apple1ACI::read(OEAddress address)
{
    if (address & 0x80)
    {
        // Noise rejection
        if (audioCodec->read(0) >= threshold)
        {
            address &= ~0x1;
            
            threshold = 0x80 - noiseRejection;
        }
        else
            threshold = 0x80 + noiseRejection;
    }
    
    toggleSpeaker();
    
    return rom->read(address);
}

void Apple1ACI::write(OEAddress address, OEUInt8 value)
{
    toggleSpeaker();
}

void Apple1ACI::mapMMU(int message)
{
    MemoryMap ioMap;
    ioMap.component = this;
    ioMap.startAddress = 0xc000;
    ioMap.endAddress = 0xc0ff;
    ioMap.read = true;
    ioMap.write = true;
    
    MemoryMap romMap;
    romMap.component = rom;
    romMap.startAddress = 0xc100;
    romMap.endAddress = 0xc1ff;
    romMap.read = true;
    romMap.write = false;
    
    if (mmu)
    {
        mmu->postMessage(this, message, &ioMap);
        mmu->postMessage(this, message, &romMap);
    }
}

void Apple1ACI::toggleSpeaker()
{
    audioLevel = (audioLevel == 0x80) ? 0xc0 : 0x80;
    
    // Write in stereo
    audioCodec->write(0, audioLevel);
    audioCodec->write(1, audioLevel);
}

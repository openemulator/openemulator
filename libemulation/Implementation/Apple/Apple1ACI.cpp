
/**
 * libemulation
 * Apple-1 ACI
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 ACI (Apple Cassette Interface)
 */

#include <math.h>

#include "Apple1ACI.h"

#include "MemoryInterface.h"

Apple1ACI::Apple1ACI()
{
    volume = 1;
    noiseRejection = 0.04;
    
    rom = NULL;
    mmu = NULL;
    audioCodec = NULL;
    
    inputCurrentThreshold = 0x80;
    
    outputState = false;
}

bool Apple1ACI::setValue(string name, string value)
{
    if (name == "volume")
        volume = getFloat(value);
    else if (name == "noiseRejection")
        noiseRejection = getFloat(value);
    else
        return false;
    
    return true;
}

bool Apple1ACI::getValue(string name, string& value)
{
    if (name == "volume")
        value = getString(volume);
    else if (name == "noiseRejection")
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
    
    mapMMU(MMU_MAP_MEMORY);
    
    return true;
}

void Apple1ACI::update()
{
    if (volume != 0)
        outputHighLevel = 16384 * pow(10.0, (volume - 1.0) * 40.0 / 20.0);
    else
        outputHighLevel = 0;
}

void Apple1ACI::dispose()
{
    mapMMU(MMU_UNMAP_MEMORY);
}

OEUInt8 Apple1ACI::read(OEAddress address)
{
    if (address & 0x80)
    {
        // Noise rejection
        if (audioCodec->read(0) >= inputCurrentThreshold)
        {
            address &= ~0x1;
            
            inputCurrentThreshold = 0x80 - inputTriggerThreshold;
        }
        else
            inputCurrentThreshold = 0x80 + inputTriggerThreshold;
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
    outputState = !outputState;
    
    // Stereo
    if (outputState)
    {
        audioCodec->write16(0, outputHighLevel);
        audioCodec->write16(1, outputHighLevel);
    }
    else
    {
        audioCodec->write16(0, 0);
        audioCodec->write16(1, 0);
    }
}


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
    rom = NULL;
}

bool Apple1ACI::setRef(string name, OEComponent *ref)
{
    if (name == "rom")
        rom = ref;
    else
        return Audio1Bit::setRef(name, ref);
    
    return true;
}

bool Apple1ACI::init()
{
    if (!Audio1Bit::init())
        return false;
    
    OECheckComponent(rom);
    
    return true;
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

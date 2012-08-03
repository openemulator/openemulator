
/**
 * libemulation
 * Apple II Address Decoder
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II Address Decoder
 */

#include "AppleIIAddressDecoder.h"

#include "AppleIIInterface.h"

enum
{
    TYPE_II,
    TYPE_III,
    TYPE_IIE,
};

AppleIIAddressDecoder::AppleIIAddressDecoder() : AddressDecoder()
{
    type = TYPE_II;
}

bool AppleIIAddressDecoder::setValue(string name, string value)
{
    if (name == "type")
    {
        if (value == "II")
            type = TYPE_II;
        else if (value == "III")
            type = TYPE_III;
        else if (value == "IIe")
            type = TYPE_IIE;
    }
    else
        return AddressDecoder::setValue(name, value);
    
    return true;
}

bool AppleIIAddressDecoder::setRef(string name, OEComponent *ref)
{
    if (name == "ram")
        ram = ref;
    else if (name == "auxMemory")
        auxMemory = ref;
    else if (name == "io")
        io = ref;
    else if (name == "slot0")
        slot[0] = ref;
    else if (name == "slot1")
        slot[1] = ref;
    else if (name == "slot2")
        slot[2] = ref;
    else if (name == "slot3")
        slot[3] = ref;
    else if (name == "slot4")
        slot[4] = ref;
    else if (name == "slot5")
        slot[5] = ref;
    else if (name == "slot6")
        slot[6] = ref;
    else if (name == "slot7")
        slot[7] = ref;
    else if (name == "romCD")
        romCD = ref;
    else if (name == "romEF")
        romEF = ref;
    else if (name == "rom")
        rom = ref;
    else
        return AddressDecoder::setRef(name, ref);
    
    return true;
}

bool AppleIIAddressDecoder::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case ADDRESSDECODER_MAP:
            return addMemoryMap((MemoryMap *) data);
            
        case ADDRESSDECODER_UNMAP:
            return removeMemoryMap((MemoryMap *) data);
            
        case APPLEII_MAP_SLOT:
            return addMemoryMap((MemoryMap *) data);
            
        case APPLEII_UNMAP_SLOT:
            return removeMemoryMap((MemoryMap *) data);
    }
    
    return false;
}

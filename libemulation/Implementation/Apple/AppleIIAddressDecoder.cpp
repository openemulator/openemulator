
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

AppleIIAddressDecoder::AppleIIAddressDecoder() : AddressDecoder()
{
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

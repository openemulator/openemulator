
/**
 * libemulation
 * Apple II Address Decoder
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II Address Decoder
 */

#include "AddressDecoder.h"

class AppleIIAddressDecoder : public AddressDecoder
{
public:
	AppleIIAddressDecoder();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
    MemoryMaps ioMemoryMaps;
    
    void updateReadWriteMap(OEAddress startAddress, OEAddress endAddress);
};

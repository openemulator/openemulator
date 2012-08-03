
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
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
    OEComponent *ram;
    OEComponent *auxMemory;
    OEComponent *io;
    OEComponent *slot[8];
    OEComponent *romCD;
    OEComponent *romEF;
    OEComponent *rom;
    
    OEInt type;
};


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
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
    void write(OEAddress address, OEUInt8 value);
    
private:
    OEComponent *video;
    
    vector<int> videoRefresh;
    
    int *videoRefreshp;
    
    OEData dummyVRAM;
    
    OEUInt8 *getMemory(OEAddress startAddress, OEAddress endAddress);
};


/**
 * libemulation
 * Apple III Address Decoder
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III Address Decoder
 */

#include "AddressDecoder.h"

class AppleIIIAddressDecoder : public AddressDecoder
{
public:
	AppleIIIAddressDecoder();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
private:
    OEComponent *memory;
    OEComponent *io;
    OEComponent *slot[8];
    OEComponent *rom;
    OEComponent *memoryFF00;
    
    MemoryMaps ioMemoryMaps;
    MemoryMaps ramMemoryMaps;
    MemoryMap *ioC500Map;
    MemoryMap *ramC000Map;
    MemoryMap *ramD000Map;
    MemoryMap *ramF000Map;
    MemoryMap *ramFF00Map;
    
    bool ioEnabled;
    
    MemoryMap ff00MemoryMap;
    MemoryMap ffc0MemoryMap;
    MemoryMap fff0MemoryMap;
    
    void updateMemoryMaps(OEAddress startAddress, OEAddress endAddress);
    
    void setSlot(OEInt index, OEComponent *ref);
    
    bool setMemoryControl(OEInt value);
    bool setAppleIIMode(bool value);
};

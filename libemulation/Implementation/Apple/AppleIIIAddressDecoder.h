
/**
 * libemulation
 * Apple III Address Decoder
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III Address Decoder
 */

#ifndef _APPLEIIIADDRESSDECODER_H
#define _APPLEIIIADDRESSDECODER_H

#include "AddressDecoder.h"

class AppleIIIAddressDecoder : public AddressDecoder
{
public:
	AppleIIIAddressDecoder();
    
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    OEComponent *bankSwitcher;
    OEComponent *io;
    OEComponent *rom;
    OEComponent *memoryFF00;
    OEComponent *systemControl;
    
    OEComponent *slot[8];
    
    MemoryMaps appleIIIMemoryMaps;
    MemoryMaps ioMemoryMaps;
    MemoryMaps ramMemoryMaps;
    MemoryMap *ioC500Map;
    MemoryMap *ramC000Map;
    MemoryMap *ramD000Map;
    MemoryMap *ramF000RMap;
    MemoryMap *ramF000WMap;
    MemoryMap *ramFF00Map;
    
    MemoryMap ff00RMemoryMap;
    MemoryMap ff00WMemoryMap;
    MemoryMap ffc0MemoryMap;
    MemoryMap fff0MemoryMap;
    MemoryMap fff0RMemoryMap;
    MemoryMap fff0WMemoryMap;
    
    OEChar environment;
    bool appleIIMode;
    
    void updateReadWriteMap(OEAddress startAddress, OEAddress endAddress);
    
    void setSlot(OEInt index, OEComponent *ref);
    
    void updateAppleIIIMemoryMaps();
    
    bool setEnvironment(OEChar value);
    bool setAppleIIMode(bool value);
};

#endif

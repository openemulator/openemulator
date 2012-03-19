
/**
 * libemulation
 * Address decoder
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address decoder
 */

#ifndef _ADDRESSDECODER_H
#define _ADDRESSDECODER_H

#include "OEComponent.h"

#include "MemoryInterface.h"

class AddressDecoder : public OEComponent
{
public:
    AddressDecoder();
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
    
protected:
    OEAddress addressSize;
    OEAddress blockSize;
    
    MemoryMaps staticMemoryMaps;
    MemoryMaps dynamicMemoryMaps;
    
    OEComponent **readMapp;
    OEComponent **writeMapp;
    
    OEAddress addressMask;
    
private:
    OEComponent *floatingBus;
    MemoryMapsRef ref;
    MemoryMapsConf conf;
    
    OEComponents readMap;
    OEComponents writeMap;
    
    void mapMemory(MemoryMap& value);
    void clear();
    void updateMemoryMaps(MemoryMaps& value, OEAddress startAddress, OEAddress endAddress);
    void updateMemoryMaps(OEAddress startAddress, OEAddress endAddress);
    
    bool addMemoryMaps(MemoryMaps *value);
    bool removeMemoryMaps(MemoryMaps *value);
};

#endif

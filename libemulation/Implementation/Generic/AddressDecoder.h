
/**
 * libemulation
 * Address decoder
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an address decoder
 */

#ifndef _ADDRESSDECODER_H
#define _ADDRESSDECODER_H

#include "OEComponent.h"

typedef enum
{
    ADDRESSDECODER_MAP,
} AddressDecoderMessage;

typedef struct
{	
    OEComponent *component;
    
    OEAddress startAddress;
    OEAddress endAddress;
    
    bool read;
    bool write;
} AddressDecoderMap;

typedef vector<AddressDecoderMap> AddressDecoderMaps;
typedef map<string, string> AddressDecoderConf;
typedef map<string, OEComponent *> AddressDecoderRef;

class AddressDecoder : public OEComponent
{
public:
    AddressDecoder();
    
    bool setValue(string name, string value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    bool postMessage(OEComponent *sender, int event, void *data);
    
    OEUInt8 read(OEAddress address);
    void write(OEAddress address, OEUInt8 value);
    
private:
    OEAddress addressSize;
    OEAddress blockSize;
    OEComponent *floatingBus;
    
    AddressDecoderConf conf;
    AddressDecoderRef ref;
    AddressDecoderMaps pendingMaps;
    
    OEAddress addressMask;
    OEComponents readMap;
    OEComponents writeMap;
    
    OEComponents defaultReadMap;
    OEComponents defaultWriteMap;
    
    void mapDecoderMap(AddressDecoderMap *theMap);
    
    bool getDecoderMap(AddressDecoderMap& theMap, OEComponent *component, string value);
    bool mapConf(OEComponent *component, string conf);
};

#endif

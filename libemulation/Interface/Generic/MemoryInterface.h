
/**
 * libemulation
 * Memory interface
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the address interfaces
 */

#ifndef _ADDRESSINTERFACE_H
#define _ADDRESSINTERFACE_H

#include <list>

#include "OEComponent.h"

typedef enum
{
    ADDRESSDECODER_MAP_MEMORYMAPS,
    ADDRESSDECODER_UNMAP_MEMORYMAPS,
    
    ADDRESSDECODER_END,
} AddressDecoderMessage;

typedef enum
{
    RAM_GET_DATA,
} RAMMessage;

typedef struct
{	
    OEComponent *component;
    
    OEAddress startAddress;
    OEAddress endAddress;
    
    bool read;
    bool write;
} MemoryMap;

typedef list<MemoryMap> MemoryMaps;

typedef map<string, string> MemoryMapsConf;
typedef map<string, OEComponent *> MemoryMapsRef;

bool appendMemoryMaps(MemoryMaps& theMaps,
                      OEComponent *component,
                      string value);
bool validateMemoryMapsConf(MemoryMaps& theMaps,
                            OEAddress blockSize,
                            OEAddress addressMask);

#endif

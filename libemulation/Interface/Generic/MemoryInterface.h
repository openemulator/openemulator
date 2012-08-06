
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
bool validateMemoryMaps(MemoryMaps& theMaps,
                        OEAddress blockSize,
                        OEAddress addressMask);

typedef struct
{
    OEAddress startAddress;
    OEAddress endAddress;
    
	OESLong offset;
} AddressOffsetMap;

typedef list<AddressOffsetMap> AddressOffsetMaps;

typedef enum
{
    ADDRESSDECODER_MAP,
    ADDRESSDECODER_UNMAP,
    ADDRESSDECODER_END,
} AddressDecoderMessage;

typedef enum
{
    ADDRESSMAPPER_SELECT,
} AddressMapperMessage;

typedef enum
{
    ADDRESSMASKER_SET_MASK,
} AddressMaskerMessage;

typedef enum
{
    ADDRESSOFFSET_MAP,
} AddressOffsetMessage;

typedef enum
{
    RAM_GET_DATA,
} RAMMessage;

typedef enum
{
    VRAM_WILL_CHANGE,
} VRAMNotification;

#endif

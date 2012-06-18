
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
    ADDRESSDECODER_MAP,
    ADDRESSDECODER_UNMAP,
} AddressDecoderMessage;

typedef enum
{
    ADDRESSMAPPER_MAP,
} AddressMapperMessage;

typedef enum
{
    ADDRESSMASKER_SET_MASK,
} AddressMaskerMessage;

typedef enum
{
    ADDRESSOFFSET_SET_OFFSET,
} AddressOffsetMessage;

typedef enum
{
    RAM_GET_DATA,
    RAM_END,
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
bool validateMemoryMaps(MemoryMaps& theMaps,
                        OEAddress blockSize,
                        OEAddress addressMask);

typedef enum
{
    BANKSWITCHEDRAM_MAP = RAM_END,
} BankSwitchedRAMMessage;

typedef struct
{
    OEAddress startAddress;
    OEAddress endAddress;
    
	OESLong offset;
} BankSwitchedRAMMap;

typedef enum
{
    VIDEORAM_WILL_CHANGE,
} VideoRAMNotification;

#endif

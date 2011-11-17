
/**
 * libemulation
 * Memory interface
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
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
} AddressDecoderMessage;

typedef enum
{
    MMU_MAP,
    MMU_UNMAP,
} MMUMessage;

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

#endif

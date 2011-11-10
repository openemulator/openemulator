
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

#include "OEComponent.h"

typedef enum
{
    ADDRESSDECODER_MAP,
    ADDRESSDECODER_CLEAR,
} AddressDecoderMessage;

typedef enum
{
    MMU_MAP,
    MMU_UNMAP,
} MMUMessage;

typedef enum
{
    RAM_GET_MEMORYSIZE,
    RAM_GET_MEMORY,
    RAM_SET_MEMORY,
} RAMMessage;

typedef struct
{	
    OEComponent *component;
    
    OEAddress startAddress;
    OEAddress endAddress;
    
    bool read;
    bool write;
} MemoryMap;

#endif

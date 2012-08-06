
/**
 * libemulation
 * Apple III Interface
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the Apple III interface
 */

#ifndef _APPLEIIIINTERFACE_H
#define _APPLEIIIINTERFACE_H

#include "CPUInterface.h"
#include "AppleIIInterface.h"

#define APPLEIII_SYSTEMBANK 2

typedef enum
{
    APPLEIII_SET_EXTENDEDMEMORYENABLE = CPU_END,
} AppleIIIMOS6502Message;

// Memory control
#define APPLEIII_RAMWP      (1 << 0)
#define APPLEIII_IO         (1 << 1)
#define APPLEIII_ROM        (1 << 2)

typedef enum
{
    APPLEIII_SET_MEMORYCONTROL = APPLEII_ADDRESSDECODER_END,
    APPLEIII_SET_APPLEIIMODE,
} AppleIIIAddressDecoderMessage;

#endif

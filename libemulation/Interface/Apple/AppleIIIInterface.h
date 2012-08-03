
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

#define APPLEIII_SYSTEMBANK 2

typedef enum
{
    APPLEIII_SET_EXTENDEDMEMORYENABLE = CPU_END,
} AppleIIIMOS6502Message;

#endif

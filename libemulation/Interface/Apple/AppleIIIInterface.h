
/**
 * libemulation
 * Apple II Interface
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the Apple II interface
 */

#ifndef _APPLEIIIINTERFACE_H
#define _APPLEIIIINTERFACE_H

#include "AppleIIInterface.h"

#define APPLEIII_SYSTEMBANK 2

typedef enum
{
    APPLEIII_SET_ZEROPAGE = CPU_END,
} AppleIIIMOS6502Message;

// Apple III environment register
#define APPLEIII_ROMSEL1            (1 << 0)
#define APPLEIII_ROMSEL2            (1 << 1)
#define APPLEIII_NORMALSTACK        (1 << 2)
#define APPLEIII_RAMWP              (1 << 3)
#define APPLEIII_RESETENABLE        (1 << 4)
#define APPLEIII_VIDEOENABLE        (1 << 5)
#define APPLEIII_IOENABLE           (1 << 6)
#define APPLEIII_SLOWSPEED          (1 << 7)

// Apple III ram bank register
#define APPLEIII_NOT_APPLEIIMODE    (1 << 6)

// Apple III sound register
#define APPLEIII_BL                 (1 << 6)
#define APPLEIII_NOT_IONMI          (1 << 7)

typedef enum
{
    APPLEIII_GET_ENVIRONMENT,
    APPLEIII_GET_APPLEIIMODE,
    APPLEIII_GET_NOT_IONMI,
    APPLEIII_SET_EXTENDEDRAMBANK,
} AppleIIISystemControlMessage;

typedef enum
{
    APPLEIII_ENVIRONMENT_DID_CHANGE,
    APPLEIII_APPLEIIMODE_DID_CHANGE,
} AppleIIISystemControlNotification;

#endif

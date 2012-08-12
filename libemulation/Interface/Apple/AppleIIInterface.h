
/**
 * libemulation
 * Apple II Interface
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the Apple II interface
 */

#ifndef _APPLEIIINTERFACE_H
#define _APPLEIIINTERFACE_H

#include "CanvasInterface.h"
#include "StorageInterface.h"

#include "CPUInterface.h"
#include "MemoryInterface.h"

#define APPLEII_CLOCKFREQUENCY   (14318180.0F * 65 / 912)

#define APPLEIII_SYSTEMBANK 2

typedef enum
{
    APPLEIII_SET_ZEROPAGE = CPU_END,
} AppleIIIMOS6502Message;

// Apple III environment register
#define APPLEIII_ROMSEL1        (1 << 0)
#define APPLEIII_ROMSEL2        (1 << 1)
#define APPLEIII_NORMALSTACK    (1 << 2)
#define APPLEIII_RAMWP          (1 << 3)
#define APPLEIII_RESETENABLE    (1 << 4)
#define APPLEIII_VIDEOENABLE    (1 << 5)
#define APPLEIII_IOENABLE       (1 << 6)
#define APPLEIII_SLOWSPEED      (1 << 7)

// Apple III ram bank register
#define APPLEIII_NOTAPPLEIIMODE (1 << 6)

typedef enum
{
    APPLEII_MAP_SLOT = ADDRESSDECODER_END,
    APPLEII_UNMAP_SLOT,
    APPLEII_ADDRESSDECODER_END,
} AppleIIAddressDecoderMessage;

typedef enum
{
    APPLEIII_GET_ENVIRONMENT,
    APPLEIII_GET_APPLEIIMODE,
} AppleIIISystemControlMessage;

typedef enum
{
    APPLEIII_ENVIRONMENT_DID_CHANGE,
    APPLEIII_APPLEIIMODE_DID_CHANGE,
} AppleIIISystemControlNotification;

// Apple IIe memory control
typedef enum
{
    APPLEII_SET_ALTZP = APPLEII_ADDRESSDECODER_END,
    APPLEII_SET_RAMRD,
    APPLEII_SET_RAMWRT,
    APPLEII_SET_80STORE,
    APPLEII_SET_MMUPAGE2,
    APPLEII_SET_MMUHIRES,
    APPLEII_SET_INTCXROM,
    APPLEII_SET_SLOTC3ROM,
    APPLEII_SET_LCBANK1,
    APPLEII_SET_LCRAMRD,
    APPLEII_SET_LCRAMWR,
} AppleIIeAddressDecoderMessage;

// Move this to AppleIIControlBus:
//    APPLEII_ASSERT_DISKMOTORON,
//    APPLEII_CLEAR_DISKMOTORON,

typedef enum
{
    APPLEII_REFRESH_VIDEO = CANVAS_END,
    APPLEII_READ_FLOATINGBUS,
    APPLEII_REQUEST_MONITOR,
    APPLEII_RELEASE_MONITOR,
    APPLEII_IS_MONITOR_CONNECTED,
    APPLEII_IS_COLORKILLER_ENABLED,
    APPLEIII_SET_APPLEIIVIDEO,
} AppleIIVideoMessage;

typedef enum
{
    APPLEII_COLORKILLER_DID_CHANGE = CANVAS_NOTIFICATION_END,
    APPLEII_MONITOR_DID_CHANGE,
} AppleIIVideoNotification;

typedef enum
{
    APPLEII_SET_PDL0,
    APPLEII_SET_PDL1,
    APPLEII_SET_PDL2,
    APPLEII_SET_PDL3,
    APPLEII_SET_PB3,
    APPLEII_SET_PB0,
    APPLEII_SET_PB1,
    APPLEII_SET_PB2,
    APPLEII_GET_AN0,
    APPLEII_GET_AN1,
    APPLEII_GET_AN2,
    APPLEII_GET_AN3,
    
    APPLEII_SET_C073_HANDLER,
} AppleIIGamePortMessage;

typedef enum
{
    APPLEII_AN0_DID_CHANGE,
    APPLEII_AN1_DID_CHANGE,
    APPLEII_AN2_DID_CHANGE,
    APPLEII_AN3_DID_CHANGE,
    APPLEII_DID_STROBE,
} AppleIIGamePortNotification;

typedef enum
{
    APPLEII_DISABLE_C800,
    APPLEII_IS_C800_ENABLED,
} AppleIISlotControllerMessage;

typedef enum
{
    APPLEII_C800_DID_CHANGE,
} AppleIISlotControllerNotification;

typedef enum
{
    APPLEII_ASSERT_DRIVEENABLE = STORAGE_END,
    APPLEII_CLEAR_DRIVEENABLE,
    APPLEII_SET_PHASECONTROL,
    APPLEII_SENSE_INPUT,
    APPLEII_SKIP_DATA,
} AppleIIDiskDriveMessage;

#endif

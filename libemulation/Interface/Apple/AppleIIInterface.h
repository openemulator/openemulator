
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

#include "MemoryInterface.h"

#define APPLEII_CLOCKFREQUENCY   (14318180.0F * 65 / 912)

typedef enum
{
    APPLEII_VRAMMODE_TEXT1,
    APPLEII_VRAMMODE_TEXT2,
    APPLEII_VRAMMODE_HIRES1,
    APPLEII_VRAMMODE_HIRES2,
    APPLEII_VRAMMODE_MIXED1,
    APPLEII_VRAMMODE_MIXED2,
    APPLEII_VRAMMODE_SHIRES,
} AppleIIVRAMMode;

typedef struct
{
    OEChar *textMain[2];
    OEChar *hiresMain[2];
    OEChar *textAux[2];
    OEChar *hiresAux[2];
    OEChar *hbl[2];
} AppleIIVRAM;

// MMU State
#define APPLEIIMMU_ALTZP      (1 << 0)
#define APPLEIIMMU_RAMRD      (1 << 1)
#define APPLEIIMMU_RAMWRT     (1 << 2)
#define APPLEIIMMU_80STORE    (1 << 3)
#define APPLEIIMMU_PAGE2      (1 << 4)
#define APPLEIIMMU_HIRES      (1 << 5)
#define APPLEIIMMU_INTCXROM   (1 << 6)
#define APPLEIIMMU_SLOTC3ROM  (1 << 7)
#define APPLEIIMMU_LCBANK1    (1 << 8)
#define APPLEIIMMU_LCRAMRD    (1 << 9)
#define APPLEIIMMU_LCRAMWR    (1 << 10)

typedef enum
{
    APPLEII_MAP_SLOT = ADDRESSDECODER_END,
    APPLEII_UNMAP_SLOT,
    
    APPLEII_GET_VRAM,
    
    APPLEII_SET_AUXMEMORY,
    APPLEII_SET_MMUSTATE,
    APPLEII_GET_MMUSTATE,
} AppleIIAddressDecoderMessage;

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

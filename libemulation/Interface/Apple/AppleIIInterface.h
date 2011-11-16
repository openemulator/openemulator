
/**
 * libemulation
 * Apple II MMU Interface
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the Apple II MMU interface
 */

#ifndef _APPLEIIMMUINTERFACE_H
#define _APPLEIIMMUINTERFACE_H

typedef enum
{
    APPLEIIMMU_GET_VIDEOMEMORY,
    APPLEIIMMU_MAP_SLOT,
    APPLEIIMMU_UNMAP_SLOT,
    APPLEIIMMU_MAP_SLOTEXPANSION,
    APPLEIIMMU_UNMAP_SLOTEXPANSION,
	APPLEIIMMU_MAP_MEMORY,
    APPLEIIMMU_UNMAP_MEMORY,
} AppleIIMMUMessage;

typedef enum
{
    APPLEIIMMU_VIDEOMEMORY_DID_CHANGE,
    APPLEIIMMU_SLOTEXPANSIONMEMORY_WILL_UNMAP,
} AppleIIMMUNotification;

class AppleIIMMUVideoMemory
{
public:
    AppleIIMMUVideoMemory()
    {
        text = NULL;
        hires = NULL;
        auxText = NULL;
        auxHires = NULL;
    }
    
    OEUInt8 *text;
    OEUInt8 *hires;
    OEUInt8 *auxText;
    OEUInt8 *auxHires;
};

typedef struct
{
    OEUInt32 slot;
    OEComponent *io;
    OEComponent *memory;
} AppleIIMMUSlotMap;

typedef enum
{
    APPLEIIVIDEO_GET_MODE,
} AppleIIVideoMessage;

typedef enum
{
    APPLEIIVIDEO_MODE_DID_CHANGE,
} AppleIIVideoNotification;

#define APPLEIIVIDEO_TEXT   (1 << 0)
#define APPLEIIVIDEO_MIXED  (1 << 1)
#define APPLEIIVIDEO_PAGE2  (1 << 2)
#define APPLEIIVIDEO_HIRES  (1 << 3)

typedef enum
{
	APPLEIIGAMEPORT_SET_PDL0,
	APPLEIIGAMEPORT_SET_PDL1,
	APPLEIIGAMEPORT_SET_PDL2,
	APPLEIIGAMEPORT_SET_PDL3,
	APPLEIIGAMEPORT_SET_PB0,
	APPLEIIGAMEPORT_SET_PB1,
	APPLEIIGAMEPORT_SET_PB2,
    APPLEIIGAMEPORT_GET_AN0,
    APPLEIIGAMEPORT_GET_AN1,
    APPLEIIGAMEPORT_GET_AN2,
    APPLEIIGAMEPORT_GET_AN3,
} AppleIIGamePortMessage;

typedef enum
{
    APPLEIIGAMEPORT_DID_STROBE,
    APPLEIIGAMEPORT_AN0_DID_CHANGE,
    APPLEIIGAMEPORT_AN1_DID_CHANGE,
    APPLEIIGAMEPORT_AN2_DID_CHANGE,
    APPLEIIGAMEPORT_AN3_DID_CHANGE,
} AppleIIGamePortNotification;

#endif

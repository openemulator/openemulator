
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
	APPLEIIMMU_INHIBIT_MEMORY,
	APPLEIIMMU_SET_AUX_MEMORY,
} AppleIIMMUMessage;

typedef enum
{
	APPLEIISLOTEXPANSIONMEMORY_SET_SLOT,
} AppleIISlotExpansionMemoryMessage;

#endif

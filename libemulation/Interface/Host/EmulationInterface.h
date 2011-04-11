
/**
 * libemulation
 * Emulation interface
 * (C) 2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the emulation interface.
 */

#ifndef _EMULATIONINTERFACE_H
#define _EMULATIONINTERFACE_H

typedef enum
{
	EMULATION_UPDATE,
	EMULATION_RUN_ALERT,
	EMULATION_ASSERT_RUNNING,
	EMULATION_CLEAR_RUNNING,
	EMULATION_CREATE_CANVAS,
	EMULATION_DESTROY_CANVAS,
} EmulationMessage;

typedef enum
{
	ETHERNET_DID_RECEIVE_DATA,
} EmulationNotification;

#endif

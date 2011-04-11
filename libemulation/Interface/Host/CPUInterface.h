
/**
 * libemulation
 * CPU Interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the CPU interface
 */

#ifndef _CPUINTERFACE_H
#define _CPUINTERFACE_H

typedef enum
{
	CPU_RUN_CLOCKCYCLES,
	CPU_SET_CLOCKCYCLES,
	CPU_GET_CLOCKCYCLES,
} CPUMessage;

typedef enum
{
	CPU_INSTRUCTIONCYCLE_DID_END,
} CPUNotification;

#endif

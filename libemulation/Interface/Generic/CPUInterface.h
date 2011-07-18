
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
	CPU_SET_CLOCKCYCLES,
	CPU_GET_CLOCKCYCLES,
	CPU_RUN,
} CPUMessage;

typedef enum
{
	CPU_INSTRUCTIONCYCLE_DID_END,
} CPUNotification;

#endif

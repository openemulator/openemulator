
/**
 * libemulation
 * CPU Interface
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the CPU interface
 */

// Notes:
// * setPendingCycles sets the number of cycles to be executed (OESLong)
// * getPendingCycles returns the number of remaining cycles (OESLong)
// * run executes a number of CPU cycles

#ifndef _CPUINTERFACE_H
#define _CPUINTERFACE_H

typedef enum
{
	CPU_SET_PENDINGCYCLES,
	CPU_GET_PENDINGCYCLES,
	CPU_RUN,
    CPU_END,
} CPUMessage;

#endif

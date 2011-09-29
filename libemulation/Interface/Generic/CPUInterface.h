
/**
 * libemulation
 * CPU Interface
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the CPU interface
 */

// Notes:
// * runCycles executes a number of CPU cycles, returning the number of
//   remaining cycles (OEInt64)
// * setCycles sets the number of cycles to be executed (OEInt64)
// * getCycles returns the number of remaining cycles (OEInt64)

#ifndef _CPUINTERFACE_H
#define _CPUINTERFACE_H

typedef enum
{
	CPU_RUN_CYCLES,
	CPU_SET_CYCLES,
	CPU_GET_CYCLES,
} CPUMessage;

#endif


/**
 * libemulation
 * CPU Interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the CPU interface
 */

#include "OEComponent.h"

typedef enum
{
	CPU_RUN,
	CPU_SET_CLOCK_CYCLES_LEFT,
	CPU_GET_CLOCK_CYCLES_LEFT,
} CPUMessages;

typedef enum
{
	CPU_INSTRUCTION_CYCLE_DID_FINISH,
} CPUNotifications;

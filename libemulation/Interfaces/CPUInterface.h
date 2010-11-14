
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
	CPU_SET_CYCLES_LEFT,
	CPU_GET_CYCLES_LEFT,
} CPUMessages;

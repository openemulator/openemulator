
/**
 * libemulation
 * Generic CPU
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a generic CPU
 */

#include "OEComponent.h"

// Messages
enum
{
	CPU_RUN,
	CPU_SEND_DEBUG_COMMAND,
	CPU_GET_CYCLES_LEFT,
	CPU_SET_CYCLES_LEFT,
};

class CPU : public OEComponent
{
};

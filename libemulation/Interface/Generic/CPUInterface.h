
/**
 * libemulation
 * CPU Interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the CPU interface
 */

typedef enum
{
	CPU_RUN,
	CPU_SET_REMAINING_CLOCKCYCLES,
	CPU_GET_REMAINING_CLOCKCYCLES,
} CPUMessage;

typedef enum
{
	CPU_INSTRUCTIONCYCLE_DID_END,
} CPUNotification;

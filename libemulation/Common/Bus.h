
/**
 * libemulation
 * Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a bus with clock control and reset/IRQ/NMI lines
 */

#include "OEComponent.h"

// Notifications
enum
{
	BUS_RESET_ASSERTED,
	BUS_IRQ_ASSERTED,
	BUS_NMI_ASSERTED,
};

// Messages
enum
{
	BUS_ASSERT_RESET,
	BUS_ASSERT_IRQ,
	BUS_CLEAR_IRQ,
	BUS_ASSERT_NMI,
	BUS_ADD_TIMER,
	BUS_REMOVE_TIMER,
	BUS_GET_CYCLE,
	BUS_GET_AUDIO_BUFFER_INDEX,
};

typedef struct
{
	int cyclesLeft;
	OEComponent *component;
	int event;
} BusTimer;

class Bus : public OEComponent
{
};


/**
 * libemulator
 * Apple II Events
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple II events
 */

#include "OEComponent.h"

// Messages
enum
{
	APPLEII_EVENTS_SELECT_MAINCPU = COMPONENT_USER,
	APPLEII_EVENTS_SELECT_AUXCPU,
	APPLEII_EVENTS_GET_SAMPLEINDEX,
};

// Receives HOST_AUDIO_BUFFERSTART
// It is possible to configure one CPU
// Calls the CPU
// Translates sample and CPU clocks (clocksPerSample)
// Receives IRQ and NMI requests, and routes them to the CPU
// Receives HOST_EVENTS_RESET_UP and routes it to the CPU

class AppleIIEvents : public OEComponent
{
};

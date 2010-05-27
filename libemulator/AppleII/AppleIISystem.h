
/**
 * libemulator
 * Apple II System
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II system
 */

#include "OEComponent.h"

// Messages
enum
{
	APPLEIISYSTEM_SELECT_MAINCPU,
	APPLEIISYSTEM_SELECT_AUXCPU,
	APPLEIISYSTEM_GET_SAMPLEINDEX,
};

// Receives HOST_AUDIO_BUFFERSTART
// It is possible to configure one CPU
// Calls the CPU
// Translates sample and CPU clocks (clocksPerSample)
// Receives IRQ and NMI requests, and routes them to the CPU
// Receives HOST_EVENTS_RESET_UP and routes it to the CPU

class AppleIISystem : public OEComponent
{
};

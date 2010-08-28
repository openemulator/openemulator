
/**
 * libemulation
 * Bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a bus with clock control and reset/IRQ/NMI lines
 */

#ifndef _BUS_H
#define _BUS_H

#include "AddressDecoder.h"

// Notifications
enum
{
	BUS_RESET_ASSERTED,
	BUS_IRQ_ASSERTED,
	BUS_IRQ_CLEARED,
	BUS_NMI_ASSERTED,
};

// Events
enum
{
	BUS_ASSERT_RESET = ADDRESSDECODER_EVENT_END,
	BUS_ASSERT_IRQ,
	BUS_CLEAR_IRQ,
	BUS_ASSERT_NMI,
	BUS_ADD_TIMER,
	BUS_REMOVE_TIMER,
	BUS_GET_CYCLE,
	BUS_GET_AUDIO_BUFFER_INDEX,
	BUS_REQUEST_BUS,
	BUS_RELEASE_BUS,
};

typedef struct
{
	int cyclesLeft;
	OEComponent *component;
	int event;
} BusTimer;

class Bus : public AddressDecoder
{
public:
	Bus();
	
	bool setProperty(const string &name, const string &value);
	bool connect(const string &name, OEComponent *component);
	
	void notify(OEComponent *component, int notification, void *data);
	
	bool postEvent(OEComponent *component, int event, void *data);
	
	OEUInt8 read(int address);
	
private:
	OEComponent *host;
	OEComponent *busMaster;
	
	OEComponent *cpuSel;
	
	int floatingBus;
	
	float crystal;
	float divider;
	float frequency;
	
	bool resetOnPowerOn;
	int irqCount;
	
	double phase;
	
	void updateFrequency();
};

#endif

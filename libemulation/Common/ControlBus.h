
/**
 * libemulation
 * Control bus
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a control bus with clock control and reset/IRQ/NMI lines
 */

#ifndef _CONTROLBUS_H
#define _CONTROLBUS_H

#include "OEComponent.h"

// Notifications
enum
{
	CONTROLBUS_RESET_ASSERTED,
	CONTROLBUS_IRQ_ASSERTED,
	CONTROLBUS_IRQ_CLEARED,
	CONTROLBUS_NMI_ASSERTED,
};

// Events
enum
{
	CONTROLBUS_ASSERT_RESET,
	CONTROLBUS_ASSERT_IRQ,
	CONTROLBUS_CLEAR_IRQ,
	CONTROLBUS_ASSERT_NMI,
	CONTROLBUS_ADD_TIMER,
	CONTROLBUS_REMOVE_TIMER,
	CONTROLBUS_GET_CYCLE,
	CONTROLBUS_GET_AUDIO_BUFFER_INDEX,
	CONTROLBUS_REQUEST_BUS,
	CONTROLBUS_RELEASE_BUS,
};

typedef struct
{
	int cyclesLeft;
	OEComponent *component;
	int event;
} ControlBusTimer;

class ControlBus : public OEComponent
{
public:
	ControlBus();
	
	bool setValue(string name, string value);
	bool setComponent(string name, OEComponent *component);
	
	void notify(OEComponent *component, int notification, void *data);
	
	bool postEvent(OEComponent *component, int event, void *data);
	
private:
	OEComponent *host;
	OEComponent *master;
	OEComponent *masterSocket;
	
	OEComponent *cpuSel;
	
	float crystal;
	float frequencyDivider;
	float frequency;
	
	bool resetOnPowerOn;
	int irqCount;
	
	double phase;
	
	void updateFrequency();
};

#endif


/**
 * libemulation
 * Control bus
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a control bus with clock control and reset/IRQ/NMI lines
 */

#ifndef _CONTROLBUS_H
#define _CONTROLBUS_H

#include "OEComponent.h"

typedef enum
{
	CONTROLBUS_SET_POWERSTATE,
	CONTROLBUS_GET_POWERSTATE,
	CONTROLBUS_ASSERT_RESET,
	CONTROLBUS_CLEAR_RESET,
	CONTROLBUS_ASSERT_IRQ,
	CONTROLBUS_CLEAR_IRQ,
	CONTROLBUS_ASSERT_NMI,
	CONTROLBUS_CLEAR_NMI,
	CONTROLBUS_SCHEDULE_TIMER,
	CONTROLBUS_REMOVE_TIMERS,
	CONTROLBUS_GET_CLOCKCYCLE,
	CONTROLBUS_ADD_CLOCKCYCLE,
	CONTROLBUS_GET_AUDIOBUFFERINDEX,
	CONTROLBUS_REQUEST_BUS,
	CONTROLBUS_RELEASE_BUS,
} ControlBusMessage;

typedef enum
{
	CONTROLBUS_POWERSTATE_DID_CHANGE,
	CONTROLBUS_RESET_DID_ASSERT,
	CONTROLBUS_RESET_DID_CLEAR,
	CONTROLBUS_IRQ_DID_ASSERT,
	CONTROLBUS_IRQ_DID_CLEAR,
	CONTROLBUS_NMI_DID_ASSERT,
	CONTROLBUS_NMI_DID_CLEAR,
	CONTROLBUS_TIMER_DID_FIRE,
} ControlBusNotification;

typedef enum
{
	CONTROLBUS_POWERSTATE_ON,
	CONTROLBUS_POWERSTATE_PAUSE,
	CONTROLBUS_POWERSTATE_STANDBY,
	CONTROLBUS_POWERSTATE_SLEEP,
	CONTROLBUS_POWERSTATE_HIBERNATE,
	CONTROLBUS_POWERSTATE_OFF,
} ControlBusPowerState;

class ControlBus : public OEComponent
{
public:
	ControlBus();
	
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	bool init();
    void update();
	
	bool postMessage(OEComponent *sender, int event, void *data);
	
	void notify(OEComponent *sender, int notification, void *data);
	
private:
	OEComponent *device;
	OEComponent *audio;
	OEComponent *cpu;
	OEComponent *cpuSocket;
	float crystalFrequency;
	float cpuFrequencyDivider;
	bool resetOnPowerOn;
	
	int powerState;
	float cpuFrequency;
	int resetCount;
	int irqCount;
	int nmiCount;
	
	double phase;
	
	void updateCPUFrequency();
	bool isPoweredOn(int powerState);
};

#endif

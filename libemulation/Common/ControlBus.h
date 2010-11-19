
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
#include "ControlBusInterface.h"

class ControlBus : public OEComponent
{
public:
	ControlBus();
	
	bool setValue(string name, string value);
	bool setRef(string name, OEComponent *ref);
	bool init();
	
	bool postMessage(OEComponent *sender, int event, void *data);
	
private:
	OEComponent *hostAudio;
	
	float crystal;
	float frequencyDivider;
	OEComponent *master;
	OEComponent *masterSocket;
	bool resetOnPowerOn;
	
	int powerState;
	float frequency;
	int resetCount;
	int irqCount;
	int nmiCount;
	
	double phase;
	
	void updateFrequency();
	bool isPoweredOn(int powerState);
};

#endif

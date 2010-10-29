
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
	
	void notify(OEComponent *sender, int notification, void *data);
	
	bool postMessage(OEComponent *sender, int event, void *data);
	
private:
	OEComponent *host;
	OEComponent *master;
	OEComponent *masterSocket;
	
	OEComponent *cpuSel;
	
	float crystal;
	float frequencyDivider;
	float frequency;
	
	bool resetOnPowerOn;
	int resetCount;
	int irqCount;
	int nmiCount;
	
	double phase;
	
	void updateFrequency();
};

#endif

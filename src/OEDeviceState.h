
/**
 * OpenEmulator
 * OpenEmulator device state
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator device state object.
 */

#ifndef _OEDEVICESTATE_H
#define _OEDEVICESTATE_H

#include "OEComponent.h"
#include "HostDeviceStateInterface.h"

typedef map<string, HostDeviceState> DeviceStates;

class OEDeviceState : public OEComponent
{
public:
	bool postMessage(OEComponent *sender, int message, void *data);
	
	DeviceStates getDeviceStates();
	
private:
	DeviceStates deviceStates;
};

#endif

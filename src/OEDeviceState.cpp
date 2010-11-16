
/**
 * OpenEmulator
 * OpenEmulator device state
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator device state object.
 */

#include "OEDeviceState.h"

bool OEDeviceState::postMessage(OEComponent *sender, int message, void *data)
{
	if (message == HOST_DEVICESTATE_SET)
	{
		HostDeviceStateData *deviceStateData = (HostDeviceStateData *) data;
		deviceStates[deviceStateData->deviceId] = deviceStateData->state;
	}
	else
		return false;
	
	return true;
}

DeviceStates OEDeviceState::getDeviceStates()
{
	return deviceStates;
}

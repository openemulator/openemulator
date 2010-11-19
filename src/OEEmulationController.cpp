
/**
 * OpenEmulator
 * OpenEmulator storage
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator storage object.
 */

#include "OEEmulationController.h"

#include "HostEmulationControllerInterface.h"

OEEmulationController::OEEmulationController()
{
	alertCallback = NULL;
}

void OEEmulationController::setAlertCallback(OERunAlertCallback alertCallback)
{
	this->alertCallback = alertCallback;
}

bool OEEmulationController::mount(string path)
{
	return delegate(this, HOST_EMULATIONCONTROLLER_MOUNT, &path);
}

bool OEEmulationController::validate(string path)
{
	return delegate(this, HOST_EMULATIONCONTROLLER_VALIDATE, &path);
}

bool OEEmulationController::postMessage(OEComponent *sender, int message, void *data)
{
	if (message == HOST_EMULATIONCONTROLLER_SET_DEVICEINFO)
	{
	}
	else if (message == HOST_EMULATIONCONTROLLER_ADD_CANVAS)
	{
		
	}
	else if (message == HOST_EMULATIONCONTROLLER_REMOVE_CANVAS)
	{
	}
	else if (message == HOST_EMULATIONCONTROLLER_RUN_ALERT)
	{
		if (alertCallback)
			alertCallback();
		else
			return false;
	}
	else
		return false;
	
	return true;
}

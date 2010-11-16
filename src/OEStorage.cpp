
/**
 * OpenEmulator
 * OpenEmulator storage
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an OpenEmulator storage object.
 */

#include "OEStorage.h"

OEStorage::OEStorage()
{
	alertCallback = NULL;
}

bool OEStorage::postMessage(OEComponent *sender, int message, void *data)
{
	if (message == HOST_STORAGE_RUNALERT)
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

void OEStorage::setAlertCallback(OEStorageAlertCallback alertCallback)
{
	this->alertCallback = alertCallback;
}

bool OEStorage::mount(string path)
{
	return delegate(this, HOST_STORAGE_MOUNT, &path);
}

bool OEStorage::validate(string path)
{
	return delegate(this, HOST_STORAGE_VALIDATE, &path);
}

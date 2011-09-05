
/**
 * libemulation
 * Apple Disk II
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Disk II drive
 */

#include "AppleDiskII.h"

#include "DeviceInterface.h"
#include "StorageInterface.h"

AppleDiskII::AppleDiskII()
{
	device = NULL;
	
	forceWriteProtected = false;
}

bool AppleDiskII::setValue(string name, string value)
{
	if (name == "forceWriteProtected")
		forceWriteProtected = getUInt32(value);
	else if (name == "image")
		image = value;
	else
		return false;
	
	return true;
}

bool AppleDiskII::getValue(string name, string& value)
{
	if (name == "forceWriteProtected")
		value = getString(forceWriteProtected);
	else if (name == "image")
		value = image;
	else
		return false;
	
	return true;
}

bool AppleDiskII::setRef(string name, OEComponent *ref)
{
	if (name == "device")
	{
		if (device)
			device->postMessage(this, DEVICE_REMOVE_STORAGE, this);
		device = ref;
		if (device)
			device->postMessage(this, DEVICE_ADD_STORAGE, this);
	}
	else
		return false;
	
	return true;
}

bool AppleDiskII::init()
{
	if (!device)
	{
		logMessage("ref to 'device' undefined");
		return false;
	}
	
	return true;
}

bool AppleDiskII::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case STORAGE_IS_AVAILABLE:
			return !image.size();
			
		case STORAGE_CAN_MOUNT:
			return true;
			
		case STORAGE_MOUNT:
			if (data)
			{
				string *path = (string *)data;
				image = *path;
				
				device->postMessage(this, DEVICE_UPDATE, NULL);
				
				return true;
			}
			break;
			
		case STORAGE_UNMOUNT:
			image = "";
			
			device->postMessage(this, DEVICE_UPDATE, NULL);
			
			return true;
			
		case STORAGE_GET_MOUNTPATH:
			if (data)
			{
				string *path = (string *)data;
				*path = image;
				
				return true;
			}
			break;
			
		case STORAGE_IS_LOCKED:
			return true;
			
		case STORAGE_GET_FORMATLABEL:
			if (data)
			{
				string *value = (string *)data;
				*value = "16 sectors, 35 tracks, read-only";
				
				return true;
			}
			break;
	}
	
	return false;
}

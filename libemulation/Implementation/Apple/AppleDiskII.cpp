
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
		forceWriteProtected = (OEUInt32) getUInt(value);
	else if (name == "image")
		diskImage = value;
	else if (name == "mechanism")
		mechanism = value;
	else
		return false;
	
	return true;
}

bool AppleDiskII::getValue(string name, string& value)
{
	if (name == "forceWriteProtected")
		value = getString(forceWriteProtected);
	else if (name == "image")
		value = diskImage;
	else if (name == "mechanism")
		value = mechanism;
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
		logMessage("device not connected");
		return false;
	}
	
	return true;
}

bool AppleDiskII::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case STORAGE_IS_AVAILABLE:
			return !diskImage.size();
			
		case STORAGE_CAN_MOUNT:
			return true;
			
		case STORAGE_MOUNT:
			{
				string *path = (string *)data;
				diskImage = *path;
				
				device->postMessage(this, DEVICE_UPDATE, NULL);
				
				return true;
			}
			
		case STORAGE_UNMOUNT:
			diskImage = "";
			
			device->postMessage(this, DEVICE_UPDATE, NULL);
			
			return true;
			
		case STORAGE_GET_MOUNTPATH:
			{
				string *path = (string *)data;
				*path = diskImage;
				
				return true;
			}
			
		case STORAGE_IS_LOCKED:
			return true;
			
		case STORAGE_GET_FORMATLABEL:
			{
				string *value = (string *)data;
				*value = "16 sectors, 35 tracks, read-only";
				
				return true;
			}
            
        case STORAGE_GET_SETTINGS:
        {
            DeviceSettings *settings = (DeviceSettings *)data;
            
            DeviceSetting s;
            s.component = this;
            s.name = "locked";
            s.label = "Locked";
            s.type = "checkbox";
            s.options = "";
            
            settings->push_back(s);
            
            return true;
        }
	}
	
	return false;
}

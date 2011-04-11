
/**
 * libemulation
 * Apple Disk II
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Disk II drive
 */

#include "AppleDiskII.h"

#include "EmulationInterface.h"
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
		forceWriteProtected = getInt(value);
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
		device = ref;
	else
		return false;
	
	return true;
}

bool AppleDiskII::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case STORAGE_IS_MOUNT_PERMITTED:
			return true;
			
		case STORAGE_IS_MOUNT_POSSIBLE:
			return true;
			
		case STORAGE_MOUNT:
			if (data)
			{
				string *path = (string *)data;
				image = *path;
				
				emulation->postMessage(this, EMULATION_UPDATE, NULL);
				
				return true;
			}
			break;
			
		case STORAGE_UNMOUNT:
			image = "";
			
			emulation->postMessage(this, EMULATION_UPDATE, NULL);
			
			return true;
			
		case STORAGE_GET_MOUNTPATH:
			if (data)
			{
				string *path = (string *)data;
				*path = image;
				
				return true;
			}
			break;
			
		case STORAGE_GET_STATELABEL:
			if (data)
			{
				string *value = (string *)data;
				*value = "Apple II 16 Sector (140 kiB)";
				
				return true;
			}
			break;
			
		case STORAGE_IS_LOCKED:
			return false;
	}
	
	return false;
}

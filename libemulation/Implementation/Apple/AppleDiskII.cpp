
/**
 * libemulation
 * Apple Disk II
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Disk II drive
 */

#include "AppleDiskII.h"
#include "Emulation.h"
#include "StorageInterface.h"

AppleDiskII::AppleDiskII()
{
	emulation = NULL;
	
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

bool AppleDiskII::getValue(string name, string &value)
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
	if (name == "emulation")
	{
		if (emulation)
			emulation->postMessage(this,
								   EMULATION_REMOVE_STORAGE,
								   NULL);
		emulation = ref;
		if (emulation)
			emulation->postMessage(this,
								   EMULATION_ADD_STORAGE,
								   NULL);
	}
	else
		return false;
	
	return true;
}

bool AppleDiskII::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
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
			
		case STORAGE_IS_IMAGE_SUPPORTED:
			return false;
			
		case STORAGE_IS_MOUNTED:
			return (image != "");
			
		case STORAGE_IS_WRITABLE:
			return true;
			
		case STORAGE_IS_LOCKED:
			return false;
			
		case STORAGE_GET_IMAGE_PATH:
			if (data)
			{
				string *path = (string *)data;
				*path = image;
				
				return true;
			}
			break;
			
		case STORAGE_GET_IMAGE_FORMAT:
			if (data)
			{
				string *value = (string *)data;
				*value = "Apple II 16 Sector";
				
				return true;
			}
			break;
			
		case STORAGE_GET_IMAGE_CAPACITY:
			if (data)
			{
				OEUInt64 *value = (OEUInt64 *)data;
				*value = 35 * 16 * 256;
				
				return true;
			}
			break;
	}
	
	return OEComponent::postMessage(sender, message, data);
}


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
				
				return true;
			}
			break;
			
		case STORAGE_UNMOUNT:
			image = "";
			return true;
			
		case STORAGE_IS_MOUNTABLE:
			if (data)
			{
				bool *value = (bool *) data;
				*value = true;
				
				return true;
			}
			break;
			
		case STORAGE_IS_MOUNTED:
			if (data)
			{
				bool *value = (bool *) data;
				*value = (image != "");
				
				return true;
			}
			break;
			
		case STORAGE_IS_WRITABLE:
			if (data)
			{
				bool *value = (bool *) data;
				*value = true;
				
				return true;
			}
			break;
			
		case STORAGE_IS_LOCKED:
			if (data)
			{
				bool *value = (bool *) data;
				*value = false;
				
				return true;
			}
			break;
			
		case STORAGE_GET_PATH:
			if (data)
			{
				string *path = (string *)data;
				*path = image;
				
				return true;
			}
			break;
			
		case STORAGE_GET_FORMAT:
			if (data)
			{
				string *value = (string *) data;
				*value = "Apple 16-Sector";
				
				return true;
			}
			break;
			
		case STORAGE_GET_CAPACITY:
			if (data)
			{
				OEUInt64 *value = (OEUInt64 *) data;
				*value = 35 * 16 * 256;
				
				return true;
			}
			break;
	}
	
	return OEComponent::postMessage(sender, message, data);
}

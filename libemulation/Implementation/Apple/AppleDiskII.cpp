
/**
 * libemulation
 * Apple Disk II
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Disk II drive
 */

#include "AppleDiskII.h"

#include "DeviceInterface.h"
#include "AudioPlayerInterface.h"

#include "AppleIIInterface.h"

AppleDiskII::AppleDiskII()
{
	device = NULL;
    
	forceWriteProtected = false;
}

bool AppleDiskII::setValue(string name, string value)
{
	if (name == "image")
		diskImage = value;
	else if (name == "forceWriteProtected")
		forceWriteProtected = (OEUInt32) getUInt(value);
	else if (name == "mechanism")
		mechanism = value;
    else if (name.substr(0, 5) == "sound")
        sound[name.substr(5)] = OESound(value);
	else
		return false;
	
	return true;
}

bool AppleDiskII::getValue(string name, string& value)
{
	if (name == "image")
		value = diskImage;
	else if (name == "forceWriteProtected")
		value = getString(forceWriteProtected);
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
	else if (name == "drivePlayer")
        drivePlayer = ref;
    else if (name == "headPlayer")
        headPlayer = ref;
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
			return false;
			
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
            s.name = "writeProtect";
            s.label = "Write Protected";
            s.type = "checkbox";
            s.options = "";
            
            settings->push_back(s);
            
            return true;
        }
            
        case APPLEII_CLEAR_DRIVEENABLE:
        {
            if (drivePlayer)
                drivePlayer->postMessage(this, AUDIOPLAYER_PAUSE, NULL);
            
            string image = "images/Apple/Apple Disk II.png";
            device->postMessage(this, DEVICE_SET_IMAGEPATH, &image);
            device->postMessage(this, DEVICE_UPDATE, NULL);
            
            return true;
        }
        case APPLEII_ASSERT_DRIVEENABLE:
        {
            if (drivePlayer)
            {
                drivePlayer->postMessage(this, AUDIOPLAYER_SET_SOUND, &sound["AlpsDrive"]);
                drivePlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
            }
            
            string image = "images/Apple/Apple Disk II In Use.png";
            device->postMessage(this, DEVICE_SET_IMAGEPATH, &image);
            device->postMessage(this, DEVICE_UPDATE, NULL);
            
            return true;
        }
        case APPLEII_IS_WRITE_PROTECTED:
            
            return true;
	}
	
	return false;
}


/**
 * libemulation
 * Apple Disk II
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple Disk II drive
 */

/**
 * Floppy Disk Drive Notes
 *
 * A floppy disk drive has a head stepper motor with 4 discretely controlled
 * magnetic phases. Of the 16 possible head phase states, 12 map to 8 net
 * phase vectors, 3 to undefined behaviour, and one to the off state.
 * The stepper motor has an inertial time constant of approx. 2 ms.
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
    else if (name == "track")
        trackIndex = (OEUInt32) getUInt(value);
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
	else if (name == "track")
		value = getString(trackIndex);
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
    
    updateSound();
    
	return true;
}

void AppleDiskII::update()
{
    updateSound();
}

bool AppleDiskII::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case STORAGE_IS_AVAILABLE:
			return !diskImage.size();
			
		case STORAGE_CAN_MOUNT:
            // To-Do: check image compatibility
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
            // To-Do: build label
            *((string *)data) = "16 sectors, 35 tracks, read-write";
            
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
                drivePlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
            
            string image = "images/Apple/Apple Disk II In Use.png";
            device->postMessage(this, DEVICE_SET_IMAGEPATH, &image);
            device->postMessage(this, DEVICE_UPDATE, NULL);
            
            return true;
        }
            
        case APPLEII_SET_PHASECONTROL:
            setPhaseControl(*((OEUInt32 *)data));
            
            return true;
            
        case APPLEII_IS_WRITE_PROTECTED:
            *((bool *)data) = isWriteProtected;
            
            return true;
	}
	
	return false;
}

void AppleDiskII::updateSound()
{
    OESound *drivePlayerSound = NULL;
    OESound *headPlayerSound = NULL;
    
    if (sound.count(mechanism + "Drive"))
        drivePlayerSound = &sound[mechanism + "Drive"];
    if (sound.count(mechanism + "Head"))
        headPlayerSound = &sound[mechanism + "Head"];
    
    if (drivePlayer)
        drivePlayer->postMessage(this, AUDIOPLAYER_SET_SOUND, drivePlayerSound);
    if (headPlayer)
        headPlayer->postMessage(this, AUDIOPLAYER_SET_SOUND, headPlayerSound);
}

void AppleDiskII::setPhaseControl(OEUInt32 value)
{
    phaseControl = value;
    
	OEUInt32 currentPhase = trackIndex & 0x7;
	OEUInt32 nextPhase;
	
	switch (phaseControl) {
		case 0x1: case 0xb:
			nextPhase = 0;
            
			break;
            
		case 0x3:
			nextPhase = 1;
            
			break;
            
		case 0x2: case 0x7:
			nextPhase = 2;
            
			break;
            
		case 0x6:
			nextPhase = 3;
            
			break;
            
		case 0x4: case 0xe:
			nextPhase = 4;
            
			break;
            
		case 0xc:
			nextPhase = 5;
            
			break;
            
		case 0x8: case 0xd:
			nextPhase = 6;
            
			break;
            
		case 0x9:
			nextPhase = 7;
            
			break;
            
		case 0x0: case 0x5: case 0xa: case 0xf:
			nextPhase = currentPhase;
            
			break;
	}
	
	OEInt32 trackDelta = ((nextPhase - currentPhase + 4) & 0x7) - 4;
	trackIndex += trackDelta;
	
	if (trackIndex < 0)
    {
		trackIndex = 0;
        
        headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	}
    else if (trackIndex > 39)
    {
		trackIndex = 39;
        
        headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	}
}


/**
 * libemulation
 * Apple 5.25" Disk Drive
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple 5.25" Disk Drive
 */

/**
 * Floppy Disk Drive Notes
 *
 * A floppy disk drive has a head stepper motor with 4 discretely controlled
 * magnetic phases. Of the 16 possible head phase states, 12 map to 8 net
 * phase vectors, 3 to undefined behaviour, and one to the off state.
 * The stepper motor has an inertial time constant of approx. 2 ms.
 */

#include "AppleDiskDrive525.h"

#include "DeviceInterface.h"
#include "AudioPlayerInterface.h"

#include "AppleIIInterface.h"

AppleDiskDrive525::AppleDiskDrive525()
{
	device = NULL;
    
	forceWriteProtected = false;
}

bool AppleDiskDrive525::setValue(string name, string value)
{
	if (name == "diskImage")
		openDiskImage(value);
    else if (name == "track")
        trackIndex = getOEInt(value);
	else if (name == "forceWriteProtected")
		forceWriteProtected = getOEInt(value);
	else if (name == "mechanism")
		mechanism = value;
    else if (name.substr(0, 5) == "sound")
        sound[name.substr(5)] = OESound(value);
	else
		return false;
	
	return true;
}

bool AppleDiskDrive525::getValue(string name, string& value)
{
	if (name == "diskImage")
		value = diskImagePath;
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

bool AppleDiskDrive525::setRef(string name, OEComponent *ref)
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

bool AppleDiskDrive525::init()
{
	if (!device)
	{
		logMessage("device not connected");
        
		return false;
	}
    
    updateSound();
    
	return true;
}

void AppleDiskDrive525::update()
{
    updateSound();
}

bool AppleDiskDrive525::postMessage(OEComponent *sender, int message, void *data)
{
	switch(message)
	{
		case STORAGE_IS_AVAILABLE:
			return !diskImagePath.size();
			
		case STORAGE_CAN_MOUNT:
            // To-Do: check image compatibility
			return true;
			
		case STORAGE_MOUNT:
            if (openDiskImage(*((string *)data)))
            {
                device->postMessage(this, DEVICE_UPDATE, NULL);
                
                return false;
            }
            
            return true;
			
		case STORAGE_UNMOUNT:
            if (closeDiskImage())
            {
                device->postMessage(this, DEVICE_UPDATE, NULL);
                
                return false;
            }
            
            return true;
			
		case STORAGE_GET_MOUNTPATH:
            *((string *)data) = diskImagePath;
            
            return true;
			
		case STORAGE_IS_LOCKED:
			return false;
			
		case STORAGE_GET_FORMATLABEL:
            *((string *)data) = diskStorage.getFormatLabel();
            
            return true;
            
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
            setPhaseControl(*((OEInt *)data));
            
            return true;
            
        case APPLEII_IS_WRITE_PROTECTED:
            *((bool *)data) = isWriteProtected;
            
            return true;
	}
	
	return false;
}

OEChar AppleDiskDrive525::read(OEAddress address)
{
    return 0;
}

void AppleDiskDrive525::write(OEAddress address, OEChar value)
{
}

void AppleDiskDrive525::updateSound()
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

void AppleDiskDrive525::setPhaseControl(OEInt value)
{
    phaseControl = value;
    
	OEInt currentPhase = trackIndex & 0x7;
	OEInt nextPhase;
	
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
	
	OESInt trackDelta = ((nextPhase - currentPhase + 4) & 0x7) - 4;
    
    OESInt nextTrackIndex = trackIndex + trackDelta;
	
	if (nextTrackIndex < 0)
    {
		trackIndex = 0;
        
        headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	}
    else if (nextTrackIndex > 39)
    {
		trackIndex = 39;
        
        headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	}
    else
        trackIndex = nextTrackIndex;
    
    if (trackDelta)
    {
        
    }
}

bool AppleDiskDrive525::openDiskImage(string path)
{
    if (!diskStorage.open(path))
        return false;
    
    diskImagePath = path;
    
    return true;
}

bool AppleDiskDrive525::closeDiskImage()
{
    if (!diskStorage.close())
        return false;
    
    diskImagePath = "";
    
    return true;
}

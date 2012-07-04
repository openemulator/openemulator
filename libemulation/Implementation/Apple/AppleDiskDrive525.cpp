
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

#define DRIVE_TRACKNUM   (40 * 4)

AppleDiskDrive525::AppleDiskDrive525()
{
	device = NULL;
    drivePlayer = NULL;
    headPlayer = NULL;
    
	forceWriteProtected = false;
    
    phaseControl = 0;
    trackIndex = 0;
    
    trackDataIndex = 0;
    updateTrack();
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
			return true;
			
		case STORAGE_CAN_MOUNT:
        {
            DIApple525DiskStorage diskStorage;
            
            return diskStorage.open(*((string *)data));
        }
		case STORAGE_MOUNT:
            if (openDiskImage(*((string *)data)))
            {
                device->postMessage(this, DEVICE_UPDATE, NULL);
                
                return true;
            }
            
            return false;
			
		case STORAGE_UNMOUNT:
            if (closeDiskImage())
            {
                device->postMessage(this, DEVICE_UPDATE, NULL);
                
                return true;
            }
            
            return false;
			
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
            *((bool *)data) = !diskStorage.isWriteEnabled() || forceWriteProtected;
            
            return true;
	}
	
	return false;
}

OEChar AppleDiskDrive525::read(OEAddress address)
{
    OEChar value = trackData[trackDataIndex];
    
    trackDataIndex++;
    trackDataIndex %= trackDataSize;
    
    return value;
}

void AppleDiskDrive525::write(OEAddress address, OEChar value)
{
    trackData[trackDataIndex] = value;
    
    trackDataIndex++;
    trackDataIndex %= trackDataSize;
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
    OESInt oldTrackIndex = trackIndex;
    
    phaseControl = value;
    
    updateStepper(trackIndex, phaseControl);
	
	if (trackIndex < 0)
    {
		trackIndex = 0;
        
        headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	}
    else if (trackIndex >= DRIVE_TRACKNUM)
    {
		trackIndex = DRIVE_TRACKNUM - 1;
        
        headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	}
    
    headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
    headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
    
    logMessage(getString(trackIndex));
    
    if (trackIndex != oldTrackIndex)
        updateTrack();
}

void AppleDiskDrive525::updateStepper(OESInt& position, OEInt phaseControl)
{
	OESInt currentPhase = position & 0x7;
	OESInt nextPhase;
	
	switch (phaseControl)
    {
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
            
		default:
			nextPhase = currentPhase;
            
			break;
	}
    
    position += ((nextPhase - currentPhase + 4) & 0x7) - 4;
}

bool AppleDiskDrive525::openDiskImage(string path)
{
    if (!diskStorage.open(path))
        return false;
    
    diskImagePath = path;
    
    updateTrack();
    
    return true;
}

bool AppleDiskDrive525::closeDiskImage()
{
    bool success = diskStorage.close();
    
    diskImagePath = "";
    
    updateTrack();
    
    return success;
}

void AppleDiskDrive525::updateTrack()
{
    if (!diskStorage.readTrack(trackIndex, track))
    {
        track.clear();
        track.resize(1);
    }
    
    trackData = &track.front();
    trackDataSize = (OEInt) track.size();
    trackDataIndex %= trackDataSize;
}

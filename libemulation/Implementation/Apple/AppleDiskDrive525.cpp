
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
 * phase vectors, 3 give undefined behaviour, and one to the off state.
 * The stepper motor has an inertial time constant of approx. 2 ms.
 */

#include "AppleDiskDrive525.h"

#include "DeviceInterface.h"
#include "ControlBusInterface.h"
#include "AudioPlayerInterface.h"

#include "AppleIIInterface.h"

#define DRIVE_TRACKNUM   (40 * 4)

AppleDiskDrive525::AppleDiskDrive525()
{
	device = NULL;
    controlBus = NULL;
    drivePlayer = NULL;
    headPlayer = NULL;
    
    trackIndex = 0;
    trackDataIndex = 0;
    
    zeroCount = 0;
    
    isModified = false;
    
    updateTrack(trackIndex);
}

bool AppleDiskDrive525::setValue(string name, string value)
{
	if (name == "diskImage")
		openDiskImage(value);
    else if (name == "track")
        trackIndex = getOEInt(value);
	else if (name == "forceWriteProtected")
		diskStorage.setForceWriteProtected(getOEInt(value));
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
		value = diskStorage.getPath();
	else if (name == "track")
		value = getString(trackIndex);
	else if (name == "forceWriteProtected")
		value = getString(diskStorage.getForceWriteProtected());
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
    else if (name == "controlBus")
        controlBus = ref;
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
    
    if (!controlBus)
    {
		logMessage("controlBus not connected");
        
		return false;
    }
    
    updateSoundSet();
    
	return true;
}

void AppleDiskDrive525::update()
{
    updateSoundSet();
}

void AppleDiskDrive525::dispose()
{
    if (controlBus)
        controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, NULL);
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
            *((string *)data) = diskStorage.getPath();
            
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
        {
            if (controlBus)
            {
                phaseControl = *((OEInt *)data);
                
                controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, NULL);
                
                OELong cycles = 1000;
                
                controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
            }
            
            return true;
        }
        case APPLEII_SENSE_INPUT:
            *((bool *)data) = !diskStorage.isWriteEnabled();
            
            return true;
            
        case APPLEII_SKIP_DATA:
            trackDataIndex += *((OELong *)data);
            trackDataIndex %= trackDataSize;
            
            return true;
	}
	
	return false;
}

void AppleDiskDrive525::notify(OEComponent *sender, int notification, void *data)
{
    OESInt newTrackIndex = trackIndex;
    
    updateStepper(newTrackIndex, phaseControl);
    
	if (newTrackIndex < 0)
    {
		newTrackIndex = 0;
        
        headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	}
    else if (newTrackIndex >= DRIVE_TRACKNUM)
    {
		newTrackIndex = DRIVE_TRACKNUM - 1;
        
        headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	}
    
    if (trackIndex == newTrackIndex)
        return;
    
    headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
    headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
	
//    logMessage(getString((float) (newTrackIndex / 4.0)));
    
    updateTrack(newTrackIndex);
}

OEChar AppleDiskDrive525::read(OEAddress address)
{
    OEChar value = trackData[trackDataIndex];
    
    trackDataIndex++;
    trackDataIndex %= trackDataSize;
    
    if (value == 0xff)
    {
        zeroCount = 0;
        value = 1;
    }
    else if (value)
    {
        zeroCount = 0;
        
        // Weak bit support
        value = ((random() & 0xff) > value);
    }
    else
    {
        // MC3470 spurious bit behavior
        zeroCount++;
        if (zeroCount > 3)
			value = ((random() & 0x1f) == 0x1f);
    }
    
    return value;
}

void AppleDiskDrive525::write(OEAddress address, OEChar value)
{
    trackData[trackDataIndex] = value;
    
    trackDataIndex++;
    trackDataIndex %= trackDataSize;
    
    isModified = true;
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

void AppleDiskDrive525::updateTrack(OEInt value)
{
    if (isModified)
    {
        diskStorage.writeTrack(trackIndex, track);
        
        isModified = false;
    }
    
    trackIndex = value;
    
    if (!diskStorage.readTrack(trackIndex, track))
    {
        track.clear();
        track.resize(1);
    }
    
    trackData = &track.front();
    trackDataSize = (OEInt) track.size();
    trackDataIndex %= trackDataSize;
}

void AppleDiskDrive525::updateSoundSet()
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

bool AppleDiskDrive525::openDiskImage(string path)
{
    if (!diskStorage.open(path))
        return false;
    
    updateTrack(trackIndex);
    
    return true;
}

bool AppleDiskDrive525::closeDiskImage()
{
    bool success = diskStorage.close();
    
    updateTrack(trackIndex);
    
    return success;
}

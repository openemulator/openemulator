
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
    doorPlayer = NULL;
    drivePlayer = NULL;
    headPlayer = NULL;
    
    volume = 1.0;
    
    phaseControl = 0;
    phaseCycles = 0;
    phaseDirection = 0;
    phaseLastBump = false;
    phaseStop = false;
    phaseAlign = false;
    
    trackIndex = 0;
    trackPhase = 0;
    
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
        trackPhase = (trackIndex = getOEInt(value)) & 0x7;
	else if (name == "forceWriteProtected")
		diskStorage.setForceWriteProtected(getOEInt(value));
    else if (name == "imageDriveOff")
        imageDriveOff = value;
    else if (name == "imageDriveInUse")
        imageDriveInUse = value;
	else if (name == "mechanism")
		mechanism = value;
	else if (name == "volume")
		volume = getFloat(value);
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
	else if (name == "volume")
		value = getString(volume);
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
	else if (name == "doorPlayer")
        doorPlayer = ref;
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
    OECheckComponent(device);
    OECheckComponent(controlBus);
    
    update();
    
	return true;
}

void AppleDiskDrive525::update()
{
    updatePlayerSounds();
    
    drivePlayer->postMessage(this, AUDIOPLAYER_SET_VOLUME, &volume);
    headPlayer->postMessage(this, AUDIOPLAYER_SET_VOLUME, &volume);
}

void AppleDiskDrive525::dispose()
{
    if (controlBus)
    {
        OEInt id = 0;
        controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, &id);
    }
    
    closeDiskImage();
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
            if (drivePlayer)
                drivePlayer->postMessage(this, AUDIOPLAYER_PAUSE, NULL);
            
            if (device && (imageDriveOff != ""))
            {
                device->postMessage(this, DEVICE_SET_IMAGEPATH, &imageDriveOff);
                device->postMessage(this, DEVICE_UPDATE, NULL);
            }
            
            return true;
            
        case APPLEII_ASSERT_DRIVEENABLE:
            if (drivePlayer)
                drivePlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
            
            if (device && (imageDriveInUse != ""))
            {
                device->postMessage(this, DEVICE_SET_IMAGEPATH, &imageDriveInUse);
                device->postMessage(this, DEVICE_UPDATE, NULL);
            }
            
            return true;
            
        case APPLEII_SET_PHASECONTROL:
        {
            if (controlBus)
            {
                phaseControl = *((OEInt *)data);
                
                OEInt id = 0;
                controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, &id);
                
                ControlBusTimer timer = { 0.0005 * APPLEII_CLOCKFREQUENCY, 0 };
                controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &timer);
            }
            
            return true;
        }
        case APPLEII_SENSE_INPUT:
            *((bool *)data) = !diskStorage.isWriteEnabled();
            
            return true;
            
        case APPLEII_SKIP_DATA:
            trackDataIndex += (OEInt) *((OELong *)data);
            trackDataIndex %= trackDataSize;
            
            return true;
	}
	
	return false;
}

void AppleDiskDrive525::notify(OEComponent *sender, int notification, void *data)
{
    switch (((ControlBusTimer *)data)->id)
    {
        case 0:
        {
            OESInt newTrackIndex = trackIndex + getStepperDelta(trackIndex & 0x7, phaseControl);
            OESInt newPhaseDirection = getStepperDelta(trackPhase, phaseControl);
            
            trackPhase += newPhaseDirection;
            trackPhase &= 0x7;
            
            newPhaseDirection = (newPhaseDirection < 0) ? -1 : (newPhaseDirection > 0) ? 1 : 0;
            
            bool bump;
            
            // Sense bump
            if (newTrackIndex < 0)
            {
                newTrackIndex = 0;
                bump = true;
            }
            else if (newTrackIndex >= DRIVE_TRACKNUM)
            {
                newTrackIndex = DRIVE_TRACKNUM - 1;
                bump = true;
            }
            else
                bump = false;
             
            if (bump && !phaseLastBump)
            {
                phaseStop = true;
                
                headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
                
//                logMessage("bump start");
            }
            
            phaseLastBump = bump;
            
            // Sense alignment
            OELong cycles;
            controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
            
            phaseAlign = ((cycles - phaseCycles) < 0.016 * APPLEII_CLOCKFREQUENCY);
            phaseCycles = cycles;
            
            // Sense change of direction
            if (phaseDirection != newPhaseDirection)
            {
                phaseDirection = newPhaseDirection;
                headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
                
                phaseStop = false;
                
//                logMessage("direction start");
            }
            
            // Update sounds
            updatePlayerSounds();
            
            headPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
            
/*            {
                static OELong lastCycles = 0;
                logMessage(getString(phaseStop) + " / " + 
                           getString(phaseAlign) + " / " +
                           getString(phaseDirection) + " / " +
                           getString((float) (newTrackIndex / 4.0)) + " / " +
                           getString(cycles - lastCycles));
                lastCycles = cycles;
            }*/
            
            if (trackIndex != newTrackIndex)
                updateTrack(newTrackIndex);
            
            OEInt id = 1;
            controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, &id);
            
            ControlBusTimer timer = { 0.05 * APPLEII_CLOCKFREQUENCY, 1 };
            controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &timer);
            
            break;
        }
        case 1:
            headPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
            phaseStop = 0;
            phaseDirection = 0;
            
            break;
            
        case 2:
        {
            isOpenSound = false;
            
            updatePlayerSounds();
            
            doorPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
            doorPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
        }
    }
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

OESInt AppleDiskDrive525::getStepperDelta(OESInt phase, OEInt phaseControl)
{
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
			nextPhase = phase;
            
			break;
	}
    
    return ((nextPhase - phase + 4) & 0x7) - 4;
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

void AppleDiskDrive525::updatePlayerSounds()
{
    updatePlayerSound(doorPlayer, isOpenSound ? "Open" : "Close");
    updatePlayerSound(drivePlayer, "Drive");
    updatePlayerSound(headPlayer, (phaseStop ? (phaseAlign ? "Align" : "Stop") : "Head"));
}

void AppleDiskDrive525::updatePlayerSound(OEComponent *component, string value)
{
    OESound *playerSound = NULL;
    
    if (sound.count(mechanism + value))
        playerSound = &sound[mechanism + value];
    
    if (component)
        component->postMessage(this, AUDIOPLAYER_SET_SOUND, playerSound);
}

bool AppleDiskDrive525::openDiskImage(string path)
{
    bool wasMounted = (diskStorage.getPath() != "");
    
    if (!diskStorage.open(path))
        return false;
    
    updateTrack(trackIndex);
    
    if (doorPlayer)
    {
        if (wasMounted)
        {
            isOpenSound = true;
            
            ControlBusTimer timer = { 1.0 * APPLEII_CLOCKFREQUENCY, 2 };
            controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &timer);
        }
        else
            isOpenSound = false;
        
        updatePlayerSounds();
        
        doorPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        doorPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
    }
    
    return true;
}

bool AppleDiskDrive525::closeDiskImage()
{
    if (isModified)
        updateTrack(trackIndex);
    
    bool success = diskStorage.close();
    
    updateTrack(trackIndex);
    
    if (doorPlayer)
    {
        isOpenSound = true;
        
        updatePlayerSounds();
        
        doorPlayer->postMessage(this, AUDIOPLAYER_STOP, NULL);
        doorPlayer->postMessage(this, AUDIOPLAYER_PLAY, NULL);
    }
    
    return success;
}


/**
 * libemulation
 * Apple Disk II Interface Card
 * (C) 2011-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple Disk II interface card
 */

#include "AppleDiskIIInterfaceCard.h"

#include "AppleIIInterface.h"
#include "ControlBusInterface.h"

AppleDiskIIInterfaceCard::AppleDiskIIInterfaceCard()
{
	controlBus = NULL;
    drive[0] = &dummyDrive;
    drive[1] = &dummyDrive;
    
    phase = 0;
    driveSel = 0;
    driveOn = false;
    
    currentDrive = &dummyDrive;
}

bool AppleDiskIIInterfaceCard::setValue(string name, string value)
{
	if (name == "phase")
		phase = (OEUInt32) getUInt(value);
	else if (name == "driveSel")
		driveSel = (OEUInt32) getUInt(value);
	else if (name == "driveOn")
		driveOn = (OEUInt32) getUInt(value);
	else
		return false;
    
    return true;
}

bool AppleDiskIIInterfaceCard::getValue(string name, string& value)
{
	if (name == "phase")
		value = getString(phase);
	else if (name == "driveSel")
		value = getString(driveSel);
	else if (name == "driveOn")
		value = getString(driveOn);
	else
		return false;
	
	return true;
}

bool AppleDiskIIInterfaceCard::setRef(string name, OEComponent *ref)
{
	if (name == "controlBus")
    {
        if (controlBus)
        {
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_CLEAR);
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        }
		controlBus = ref;
        if (controlBus)
        {
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_CLEAR);
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        }
    }
	else if (name == "drive1")
		drive[0] = ref ? ref : &dummyDrive;
	else if (name == "drive2")
		drive[1] = ref ? ref : &dummyDrive;
	else
		return false;
	
	return true;
}

bool AppleDiskIIInterfaceCard::init()
{
    update();
    
    return true;
}

void AppleDiskIIInterfaceCard::update()
{
    currentDrive = &dummyDrive;
    
    updateDriveSel(driveSel);
}

void AppleDiskIIInterfaceCard::notify(OEComponent *sender, int notification, void *data)
{
    switch (notification)
    {
        case CONTROLBUS_TIMER_DID_FIRE:
            timerOn = false;
            
            break;
            
        case CONTROLBUS_RESET_DID_ASSERT:
            reset = true;
            
            setPhase(0, false);
            setPhase(1, false);
            setPhase(2, false);
            setPhase(3, false);
            setDriveOn(false);
            setDriveSel(0);
            
            break;
            
        case CONTROLBUS_RESET_DID_CLEAR:
            reset = false;
            
            break;
            
        case CONTROLBUS_POWERSTATE_DID_CHANGE:
            if (*((ControlBusPowerState *)data) != CONTROLBUS_POWERSTATE_ON)
                timerOn = false;
            else
                timerOn = driveOn;
                
            break;
    }
    
    updateDriveOn();
}

OEUInt8 AppleDiskIIInterfaceCard::read(OEAddress address)
{
    switch (address & 0xf)
    {
        case 0x0: case 0x1:
            setPhase(0, address & 0x1);
            
            break;
            
        case 0x2: case 0x3:
            setPhase(1, address & 0x1);
            
            break;
            
        case 0x4: case 0x5:
            setPhase(2, address & 0x1);
            
            break;
            
        case 0x6: case 0x7:
            setPhase(3, address & 0x1);
            
            break;
            
        case 0x8: case 0x9:
            setDriveOn(address & 0x1);
            
            break;
            
        case 0xa: case 0xb:
            setDriveSel(address & 0x1);
            
            break;
            
        case 0xc: case 0xd:
            setSequencer(false, address & 0x1);
            
            break;
            
        case 0xe: case 0xf:
            setSequencer(true, address & 0x1);
            
            break;
    }
    
    return 0;
}

void AppleDiskIIInterfaceCard::write(OEAddress address, OEUInt8 value)
{
    switch (address & 0xf)
    {
        case 0x0: case 0x1:
            setPhase(0, address & 0x1);
            
            break;
            
        case 0x2: case 0x3:
            setPhase(1, address & 0x1);
            
            break;
            
        case 0x4: case 0x5:
            setPhase(2, address & 0x1);
            
            break;
            
        case 0x6: case 0x7:
            setPhase(3, address & 0x1);
            
            break;
            
        case 0x8: case 0x9:
            setDriveOn(address & 0x1);
            
            break;
            
        case 0xa: case 0xb:
            setDriveSel(address & 0x1);
            
            break;
            
        case 0xc: case 0xd:
            setSequencer(false, address & 0x1);
            
            break;
            
        case 0xe: case 0xf:
            setSequencer(true, address & 0x1);
            
            break;
    }
}

void AppleDiskIIInterfaceCard::setPhase(OEUInt32 index, bool value)
{
    OESetBit(phase, 1 << index, value);
    
    currentDrive->postMessage(this, APPLEII_SET_PHASE, &phase);
}

void AppleDiskIIInterfaceCard::setDriveOn(bool value)
{
    if (driveOn == value)
        return;
    
    driveOn = value;
    
    controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, NULL);
    
    if (driveOn)
    {
        timerOn = true;
        
        updateDriveOn();
    }
    else
    {
        OEUInt64 cycles = 1000000;
        
        controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
    }
}

void AppleDiskIIInterfaceCard::updateDriveOn()
{
    bool motorOn = !reset && timerOn;
    
    currentDrive->postMessage(this, motorOn ?
                              APPLEII_ASSERT_DRIVEENABLE :
                              APPLEII_CLEAR_DRIVEENABLE, NULL);
}

void AppleDiskIIInterfaceCard::setDriveSel(OEUInt32 value)
{
    if (driveSel == value)
        return;
    
    updateDriveSel(value);
}

void AppleDiskIIInterfaceCard::updateDriveSel(OEUInt32 value)
{
    if (driveOn)
        currentDrive->postMessage(this, APPLEII_CLEAR_DRIVEENABLE, NULL);
    
    driveSel = value;
    currentDrive = drive[driveSel];
    
    currentDrive->postMessage(this, APPLEII_SET_PHASE, &phase);
    currentDrive->postMessage(this, driveOn ? APPLEII_ASSERT_DRIVEENABLE :
                              APPLEII_CLEAR_DRIVEENABLE, NULL);
}

void AppleDiskIIInterfaceCard::setSequencer(bool shift, bool value)
{
}

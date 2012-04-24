
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

#define SEQUENCER_LOAD      (1 << 0)
#define SEQUENCER_WRITE     (1 << 1)

#define SEQUENCER_SKIP      (8 * 64)

typedef enum
{
    SEQUENCER_READSHIFT,
    SEQUENCER_READLOAD,
    SEQUENCER_WRITESHIFT,
    SEQUENCER_WRITELOAD,
} SequencerMode;

AppleDiskIIInterfaceCard::AppleDiskIIInterfaceCard()
{
	controlBus = NULL;
    drive[0] = &dummyDrive;
    drive[1] = &dummyDrive;
    
    phaseControl = 0;
    driveSel = 0;
    driveOn = false;
    
    currentDrive = &dummyDrive;
    reset = false;
    timerOn = false;
    
    driveEnableControl = false;
    writeRequest = false;
    lastCycles = 0;
}

bool AppleDiskIIInterfaceCard::setValue(string name, string value)
{
	if (name == "phaseControl")
		phaseControl = getOEInt(value);
	else if (name == "driveOn")
		driveOn = getOEInt(value);
	else if (name == "driveSel")
		driveSel = getOEInt(value);
	else if (name == "sequencerMode")
		sequencerMode = getOEInt(value);
	else if (name == "sequencerState")
		sequencerState = getOEInt(value);
	else if (name == "dataRegister")
		dataRegister = getOEInt(value);
	else
		return false;
    
    return true;
}

bool AppleDiskIIInterfaceCard::getValue(string name, string& value)
{
	if (name == "phaseControl")
		value = getString(phaseControl);
	else if (name == "driveOn")
		value = getString(driveOn);
	else if (name == "driveSel")
		value = getString(driveSel);
	else if (name == "sequencerMode")
		value = getString(sequencerMode);
	else if (name == "sequencerState")
		value = getString(sequencerState);
	else if (name == "dataRegister")
		value = getString(dataRegister);
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
    else if (name == "floatingBus")
        floatingBus = ref;
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
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
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
            
            setPhaseControl(0, false);
            setPhaseControl(1, false);
            setPhaseControl(2, false);
            setPhaseControl(3, false);
            setDriveOn(false);
            setDriveSel(0);
            setSequencerLoad(false);
            setSequencerWrite(false);
            
            break;
            
        case CONTROLBUS_RESET_DID_CLEAR:
            reset = false;
            
            break;
            
        case CONTROLBUS_POWERSTATE_DID_CHANGE:
            if (*((ControlBusPowerState *)data) != CONTROLBUS_POWERSTATE_ON)
            {
                driveOn = false;
                timerOn = false;
            }
            
            break;
    }
    
    updateDriveEnableControl();
}

OEChar AppleDiskIIInterfaceCard::read(OEAddress address)
{
    if (driveEnableControl)
        updateSequencer(getQ3CyclesSinceLastUpdate() - 2, 0);
    
    switch (address & 0xf)
    {
        case 0x0: case 0x1:
            setPhaseControl(0, address & 0x1);
            
            break;
            
        case 0x2: case 0x3:
            setPhaseControl(1, address & 0x1);
            
            break;
            
        case 0x4: case 0x5:
            setPhaseControl(2, address & 0x1);
            
            break;
            
        case 0x6: case 0x7:
            setPhaseControl(3, address & 0x1);
            
            break;
            
        case 0x8: case 0x9:
            setDriveOn(address & 0x1);
            
            break;
            
        case 0xa: case 0xb:
            setDriveSel(address & 0x1);
            
            break;
            
        case 0xc: case 0xd:
            setSequencerWrite(address & 0x1);
            
            break;
            
        case 0xe: case 0xf:
            setSequencerLoad(address & 0x1);
            
            break;
    }
    
    updateSequencer(2, 0);
    
    if (driveEnableControl && !(address & 0x1))
        return dataRegister;
    else
        return floatingBus->read(address);
}

void AppleDiskIIInterfaceCard::write(OEAddress address, OEChar value)
{
    if (driveEnableControl)
        updateSequencer(getQ3CyclesSinceLastUpdate() - 2, value);
    
    switch (address & 0xf)
    {
        case 0x0: case 0x1:
            setPhaseControl(0, address & 0x1);
            
            break;
            
        case 0x2: case 0x3:
            setPhaseControl(1, address & 0x1);
            
            break;
            
        case 0x4: case 0x5:
            setPhaseControl(2, address & 0x1);
            
            break;
            
        case 0x6: case 0x7:
            setPhaseControl(3, address & 0x1);
            
            break;
            
        case 0x8: case 0x9:
            setDriveOn(address & 0x1);
            
            break;
            
        case 0xa: case 0xb:
            setDriveSel(address & 0x1);
            
            break;
            
        case 0xc: case 0xd:
            setSequencerWrite(address & 0x1);
            
            break;
            
        case 0xe: case 0xf:
            setSequencerLoad(address & 0x1);
            
            break;
    }
    
    if (driveEnableControl)
        updateSequencer(2, value);
}

void AppleDiskIIInterfaceCard::setPhaseControl(OEInt index, bool value)
{
    OEInt lastPhaseControl = phaseControl;
    
    OESetBit(phaseControl, 1 << index, value);
    
    if ((lastPhaseControl != phaseControl) && driveEnableControl)
        updatePhaseControl();
}

void AppleDiskIIInterfaceCard::updatePhaseControl()
{
    currentDrive->postMessage(this, APPLEII_SET_PHASECONTROL, &phaseControl);
}

void AppleDiskIIInterfaceCard::setDriveOn(bool value)
{
    if (driveOn == value)
        return;
    
    driveOn = value;
    
    if (timerOn)
    {
        controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, NULL);
        
        timerOn = false;
    }
    
    if (driveOn)
        controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &lastCycles);
    else
    {
        OELong cycles = 1.0 * APPLEII_CLOCKFREQUENCY;
        
        controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
        
        timerOn = true;
    }
    
    updateDriveEnableControl();
}

void AppleDiskIIInterfaceCard::updateDriveEnableControl()
{
    bool wasDriveEnableControl = driveEnableControl;
    
    driveEnableControl = (driveOn || timerOn) && !reset;
    
    if (driveEnableControl != wasDriveEnableControl)
        updateDriveEnabled();
}

void AppleDiskIIInterfaceCard::updateDriveEnabled()
{
    currentDrive->postMessage(this, driveEnableControl ?
                              APPLEII_ASSERT_DRIVEENABLE :
                              APPLEII_CLEAR_DRIVEENABLE, NULL);
    currentDrive->postMessage(this, writeRequest ?
                              APPLEII_ASSERT_WRITEREQUEST :
                              APPLEII_CLEAR_WRITEREQUEST, NULL);
}

void AppleDiskIIInterfaceCard::setDriveSel(OEInt value)
{
    if (driveSel == value)
        return;
    
    updateDriveSel(value);
}

void AppleDiskIIInterfaceCard::updateDriveSel(OEInt value)
{
    driveSel = value;
    
    OEComponent *lastDrive = currentDrive;
    currentDrive = drive[driveSel];
    
    if (lastDrive == currentDrive)
        return;
    
    if (driveEnableControl)
        lastDrive->postMessage(this, APPLEII_CLEAR_DRIVEENABLE, NULL);
    if (writeRequest)
        lastDrive->postMessage(this, APPLEII_CLEAR_WRITEREQUEST, NULL);
    
    updatePhaseControl();
    updateDriveEnabled();
    updateWriteRequest();
}

void AppleDiskIIInterfaceCard::setSequencerWrite(bool value)
{
    if (writeRequest == value)
        return;
    
    OESetBit(sequencerMode, SEQUENCER_WRITE, value);
    
    writeRequest = value;
    
    if (driveEnableControl)
        updateWriteRequest();
}

void AppleDiskIIInterfaceCard::setSequencerLoad(bool value)
{
    OESetBit(sequencerMode, SEQUENCER_LOAD, value);
}

void AppleDiskIIInterfaceCard::updateWriteRequest()
{
    currentDrive->postMessage(this, writeRequest ?
                              APPLEII_ASSERT_WRITEREQUEST : APPLEII_CLEAR_WRITEREQUEST,
                              NULL);
}

OELong AppleDiskIIInterfaceCard::getQ3CyclesSinceLastUpdate()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    OELong q3Cycles = (cycles - lastCycles) << 1;
    lastCycles = cycles;
    
    return q3Cycles;
}

void AppleDiskIIInterfaceCard::updateSequencer(OELong q3Cycles, OEChar value)
{
	switch (sequencerMode)
    {
		case SEQUENCER_READSHIFT:
        {
/*            if (q3Cycles > SEQUENCER_SKIP)
            {
                q3Cycles -= SEQUENCER_SKIP;
                
                OEUInt64 skippedBitNum = (q3Cycles >> 3);
                currentDrive->postMessage(this, APPLEII_SKIP_DATA, &skippedBitNum);
                
                q3Cycles = SEQUENCER_SKIP;
            }*/
            
            OELong bitNum = (q3Cycles + (sequencerState & 0x7)) >> 3;
            
			while (bitNum--)
            {
                bool bit = currentDrive->read(0);
                
                // Is QA set?
                if (dataRegister & 0x80)
                {
                    // Did we receive a new bit after QA was set?
                    if (sequencerState & 0x8)
                    {
                        sequencerState &= 0x7;
                        
                        dataRegister = 0x1;
                    }
                    else
                    {
                        // We did not receive a new bit after QA was set
                        // Is it arriving now?
                        if (bit)
                            sequencerState |= 0x8;
                        
                        continue;
                    }
                }
                
                dataRegister <<= 1;
                dataRegister |= bit;
			}
            
            sequencerState &= 0x8;
            sequencerState |= q3Cycles & 0x7;
            
			break;
        }
            
		case SEQUENCER_READLOAD:
        {
            bool isWriteProtected;
            
            currentDrive->postMessage(this, APPLEII_IS_WRITE_PROTECTED, &isWriteProtected);
            
            if (q3Cycles > 8)
                q3Cycles = 8;
            
            dataRegister >>= q3Cycles;
            
            if (isWriteProtected)
            {
                const OEChar bits[] =
                {
                    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
                };
                
                dataRegister |= bits[q3Cycles];
            }
            
            sequencerState = 0;
            
			break;
		}
            
        case SEQUENCER_WRITESHIFT:
        {
            OELong bitNum = (q3Cycles + sequencerState) >> 3;
            
			while (bitNum--)
            {
                bool bit = dataRegister & 0x80;
                currentDrive->write(0, bit);
                
                dataRegister <<= 1;
            }
            
            sequencerState = q3Cycles & 0x7;
            
            break;
        }
            
		case SEQUENCER_WRITELOAD:
            sequencerState += q3Cycles;
            sequencerState &= 0x7;
            
            if (sequencerState == 2)
                dataRegister = value;
            
			break;
	}
}

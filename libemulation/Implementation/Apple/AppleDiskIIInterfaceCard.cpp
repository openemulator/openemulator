
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

#define SEQUENCER_LOAD          (1 << 0)
#define SEQUENCER_WRITE         (1 << 1)

#define SEQUENCER_READ_SKIP     64
#define SEQUENCER_WRITE_SKIP    6656

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
    drive[2] = &dummyDrive;
    drive[3] = &dummyDrive;
    drive[4] = &dummyDrive;
    
    phaseControl = 0;
    driveSel = 0;
    driveOn = false;
    sequencerMode = 0;
    
    sequencerState = false;
    dataRegister = 0;
    
    currentDrive = &dummyDrive;
    reset = false;
    timerOn = false;
    
    driveEnableControl = false;
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
	else if (name == "drive3")
		drive[2] = ref ? ref : &dummyDrive;
	else if (name == "drive4")
		drive[3] = ref ? ref : &dummyDrive;
	else
		return false;
	
	return true;
}

bool AppleDiskIIInterfaceCard::init()
{
    OECheckComponent(controlBus);
    OECheckComponent(floatingBus);
    
    update();
    
    return true;
}

void AppleDiskIIInterfaceCard::update()
{
    updateDriveEnableControl();
    
    currentDrive = &dummyDrive;
    
    updateDriveSelection(driveSel);
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
            selectDrive(0);
            setSequencerLoad(false);
            setSequencerWrite(false);
            
            break;
            
        case CONTROLBUS_RESET_DID_CLEAR:
            reset = false;
            
            break;
            
        case CONTROLBUS_POWERSTATE_DID_CHANGE:
            if (*((ControlBusPowerState *)data) == CONTROLBUS_POWERSTATE_OFF)
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
    updateSwitches(address);
    
    if (driveEnableControl && !(address & 0x1))
        return dataRegister;
    
    return floatingBus->read(address);
}

void AppleDiskIIInterfaceCard::write(OEAddress address, OEChar value)
{
    updateSwitches(address);
    
    if (driveEnableControl && (address & 0x1))
        dataRegister = value;
}

inline void AppleDiskIIInterfaceCard::updateSwitches(OEAddress address)
{
    updateSequencer();
    
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
            selectDrive((OEInt) address & 0x1);
            
            break;
            
        case 0xc: case 0xd:
            setSequencerLoad(address & 0x1);
            
            break;
            
        case 0xe: case 0xf:
            setSequencerWrite(address & 0x1);
            
            break;
    }
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
    if (driveEnableControl)
        currentDrive->postMessage(this, APPLEII_SET_PHASECONTROL, &phaseControl);
}

void AppleDiskIIInterfaceCard::setDriveOn(bool value)
{
    if (driveOn == value)
        return;
    
    driveOn = value;
    
    if (timerOn)
    {
        OEInt id = 0;
        controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, &id);
        
        timerOn = false;
    }
    
    if (driveOn)
        controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &lastCycles);
    else
    {
        ControlBusTimer timer = { 1.0 * APPLEII_CLOCKFREQUENCY, 0};
        
        controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &timer);
        
        timerOn = true;
    }
    
    updateDriveEnableControl();
}

void AppleDiskIIInterfaceCard::updateDriveEnableControl()
{
    bool wasDriveEnableControl = driveEnableControl;
    
    driveEnableControl = (driveOn || timerOn) && !reset;
    
    if (driveEnableControl != wasDriveEnableControl)
    {
        updateDriveEnabled();
        updatePhaseControl();
    }
}

void AppleDiskIIInterfaceCard::updateDriveEnabled()
{
    currentDrive->postMessage(this, driveEnableControl ?
                              APPLEII_ASSERT_DRIVEENABLE :
                              APPLEII_CLEAR_DRIVEENABLE, NULL);
}

void AppleDiskIIInterfaceCard::selectDrive(OEInt value)
{
    if (driveSel == value)
        return;
    
    updateDriveSelection(value);
}

void AppleDiskIIInterfaceCard::updateDriveSelection(OEInt value)
{
    driveSel = value;
    
    OEComponent *lastDrive = currentDrive;
    currentDrive = drive[driveSel];
    
    if (lastDrive == currentDrive)
        return;
    
    if (driveEnableControl)
        lastDrive->postMessage(this, APPLEII_CLEAR_DRIVEENABLE, NULL);
    
    updatePhaseControl();
    updateDriveEnabled();
}

void AppleDiskIIInterfaceCard::setSequencerLoad(bool value)
{
    OESetBit(sequencerMode, SEQUENCER_LOAD, value);
}

void AppleDiskIIInterfaceCard::setSequencerWrite(bool value)
{
    OESetBit(sequencerMode, SEQUENCER_WRITE, value);
}

void AppleDiskIIInterfaceCard::updateSequencer()
{
    if (!driveEnableControl)
        return;
    
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    OELong bitNum = (cycles - (lastCycles & ~0x3)) >> 2;
    
	switch (sequencerMode)
    {
		case SEQUENCER_READSHIFT:
        {
            if (bitNum > SEQUENCER_READ_SKIP)
            {
                bitNum -= SEQUENCER_READ_SKIP;
                currentDrive->postMessage(this, APPLEII_SKIP_DATA, &bitNum);
                
                bitNum = SEQUENCER_READ_SKIP;
            }
            
			while (bitNum--)
            {
                bool bit = currentDrive->read(0);
                
                if (dataRegister & 0x80)
                {
                    if (!sequencerState)
                        sequencerState = bit;
                    else
                    {
                        sequencerState = 0;
                        dataRegister = 0x02 | bit;
                    }
                }
                else
                {
                    dataRegister <<= 1;
                    dataRegister |= bit;
                }
			}
            
			break;
        }
		case SEQUENCER_READLOAD:
        {
            currentDrive->postMessage(this, APPLEII_SKIP_DATA, &bitNum);
            
            OELong deltaCycles = cycles - lastCycles;
            
            if (deltaCycles > 4)
                deltaCycles = 4;
            
            OEChar senseInput;
            
            currentDrive->postMessage(this, APPLEII_SENSE_INPUT, &senseInput);
            
            senseInput <<= 7;
            
            for (OEInt i = 0; i < 2 * deltaCycles; i++)
            {
                dataRegister >>= 1;
                dataRegister |= senseInput;                
            }
            
            sequencerState = false;
            
			break;
		}
        case SEQUENCER_WRITESHIFT:
		case SEQUENCER_WRITELOAD:
            if (OEGetBit(phaseControl, (1 << 1)))
                currentDrive->postMessage(this, APPLEII_SKIP_DATA, &bitNum);
            else
            {
                if (bitNum > SEQUENCER_WRITE_SKIP)
                {
                    bitNum -= SEQUENCER_WRITE_SKIP;
                    currentDrive->postMessage(this, APPLEII_SKIP_DATA, &bitNum);
                    
                    bitNum = SEQUENCER_WRITE_SKIP;
                }
                
                while (bitNum--)
                {
                    currentDrive->write(0, (dataRegister & 0x80) ? 0xff : 0x00);
                    
                    dataRegister <<= 1;
                }
            }
            
            break;
	}
    
    lastCycles = cycles;
}

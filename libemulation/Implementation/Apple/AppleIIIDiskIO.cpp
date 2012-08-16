
/**
 * libemulation
 * Apple III Disk I/O
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III Disk I/O
 */

#include "AppleIIIDiskIO.h"

#include "AppleIIIInterface.h"

#define DRIVE_EXT0  (1 << 0)
#define DRIVE_EXT1  (1 << 1)
#define DRIVE_INT   (1 << 2)
#define DRIVE_AII   (1 << 3)

AppleIIIDiskIO::AppleIIIDiskIO() : AppleDiskIIInterfaceCard()
{
    systemControl = NULL;
    
    appleIIMode = false;
    
    driveSelect = 0;
}

bool AppleIIIDiskIO::setRef(string name, OEComponent *ref)
{
    if (name == "systemControl")
    {
        if (systemControl)
            systemControl->removeObserver(this, APPLEIII_APPLEIIMODE_DID_CHANGE);
        systemControl = ref;
        if (systemControl)
            systemControl->addObserver(this, APPLEIII_APPLEIIMODE_DID_CHANGE);
    }
    else
        return AppleDiskIIInterfaceCard::setRef(name, ref);
    
    return true;
}

bool AppleIIIDiskIO::init()
{
    OECheckComponent(systemControl);
    
    if (!AppleDiskIIInterfaceCard::init())
        return false;
    
    systemControl->postMessage(this, APPLEIII_GET_APPLEIIMODE, &appleIIMode);
    
    return true;
}

void AppleIIIDiskIO::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == systemControl)
    {
        appleIIMode = *((bool *)data);
        
        updateDriveSelect();
    }
    else
        AppleDiskIIInterfaceCard::notify(sender, notification, data);
}

OEChar AppleIIIDiskIO::read(OEAddress address)
{
    updateSwitches(address);
    
    if (driveEnableControl && !(address & 0x1))
        return dataRegister;
    
    return floatingBus->read(address);
}

void AppleIIIDiskIO::write(OEAddress address, OEChar value)
{
    updateSwitches(address);
    
    if (driveEnableControl && (address & 0x1))
        dataRegister = value;
}

inline void AppleIIIDiskIO::updateSwitches(OEAddress address)
{
    updateSequencer();
    
    switch (address & 0x1f)
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
            setDriveSelect(DRIVE_AII, address & 0x1);
            
            break;
            
        case 0xc: case 0xd:
            setSequencerLoad(address & 0x1);
            
            break;
            
        case 0xe: case 0xf:
            setSequencerWrite(address & 0x1);
            
            break;
            
        case 0x10: case 0x11:
            setDriveSelect(DRIVE_EXT0, address & 0x1);
            
            break;
            
        case 0x12: case 0x13:
            setDriveSelect(DRIVE_EXT1, address & 0x1);
            
            break;
            
        case 0x14: case 0x15:
            setDriveSelect(DRIVE_INT, address & 0x1);
            
            break;
            
        case 0x16: case 0x17:
            setDriveSide2(address & 0x1);
            
            break;
            
        case 0x18: case 0x19:
            setENSIO(address & 0x1);
            
            break;
        case 0x1a: case 0x1b:
            setENSEL(address & 0x1);
            
            break;
            
        case 0x1c: case 0x1d:
            setVideoCharacterWrite(address & 0x1);
            
            break;
            
        case 0x1e: case 0x1f:
            setVideoScroll(address & 0x1);
            
            break;
    }
}

void AppleIIIDiskIO::setDriveSelect(OEInt index, bool value)
{
    OESetBit(driveSelect, index, value);
    
    updateDriveSelect();
}

void AppleIIIDiskIO::updateDriveSelect()
{
    if (appleIIMode)
        selectDrive(OEGetBit(driveSelect, DRIVE_AII));
    else
    {
        if (!OEGetBit(driveSelect, DRIVE_INT))
            selectDrive(0);
        else if (driveSelect & 0x3)
            selectDrive(driveSelect & 0x3);
        else
            selectDrive(4);
    }
}

void AppleIIIDiskIO::setDriveSide2(bool value)
{
}

void AppleIIIDiskIO::setENSIO(bool value)
{
}

void AppleIIIDiskIO::setENSEL(bool value)
{
}

void AppleIIIDiskIO::setVideoCharacterWrite(bool value)
{
}

void AppleIIIDiskIO::setVideoScroll(bool value)
{
}

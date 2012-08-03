
/**
 * libemulation
 * Apple III Disk I/O
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III Disk I/O
 */

#include "AppleIIIDiskIO.h"

AppleIIIDiskIO::AppleIIIDiskIO() : AppleDiskIIInterfaceCard()
{
    driveSelect = 0;
}

OEChar AppleIIIDiskIO::read(OEAddress address)
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
            setDriveSelect(3, address & 0x1);
            
            break;
            
        case 0xc: case 0xd:
            setSequencerLoad(address & 0x1);
            
            break;
            
        case 0xe: case 0xf:
            setSequencerWrite(address & 0x1);
            
            break;

        case 0x10: case 0x11:
            setDriveSelect(0, address & 0x1);
            
            break;
            
        case 0x12: case 0x13:
            setDriveSelect(1, address & 0x1);
            
            break;
            
        case 0x14: case 0x15:
            setDriveSelect(2, address & 0x1);
            
            break;
            
        case 0x16: case 0x17:
            setDriveSide2(address & 0x1);
            
            break;
    }
    
    if (driveEnableControl && !(address & 0x1))
        return dataRegister;
    
    return floatingBus->read(address);
}

void AppleIIIDiskIO::write(OEAddress address, OEChar value)
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
            setDriveSelect(3, address & 0x1);
            
            break;
            
        case 0xc: case 0xd:
            setSequencerLoad(address & 0x1);
            
            break;
            
        case 0xe: case 0xf:
            setSequencerWrite(address & 0x1);
            
            break;
            
        case 0x10: case 0x11:
            setDriveSelect(0, address & 0x1);
            
            break;
            
        case 0x12: case 0x13:
            setDriveSelect(1, address & 0x1);
            
            break;
            
        case 0x14: case 0x15:
            setDriveSelect(2, address & 0x1);
            
            break;
            
        case 0x16: case 0x17:
            setDriveSide2(address & 0x1);
            
            break;
    }
    
    if (driveEnableControl && (address & 0x1))
        dataRegister = value;
}

void AppleIIIDiskIO::setDriveSelect(OEInt index, bool value)
{
    OESetBit(driveSelect, 1 << index, value);
    
    setDriveSel(0);
}

void AppleIIIDiskIO::setDriveSide2(bool value)
{
    
}

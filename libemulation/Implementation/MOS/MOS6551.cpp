
/**
 * libemulation
 * MOS6551
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6551 ACIA
 */

#include "MOS6551.h"

MOS6551::MOS6551()
{
    dataRegister = 0;
    statusRegister = 0;
    commandRegister = 0;
    controlRegister = 0;
}

bool MOS6551::setValue(string name, string value)
{
    if (name == "dataRegister")
        dataRegister = getOEInt(value);
    else if (name == "statusRegister")
        statusRegister = getOEInt(value);
    else if (name == "commandRegister")
        commandRegister = getOEInt(value);
    else if (name == "controlRegister")
        controlRegister = getOEInt(value);
    else
        return false;
    
    return true;
}

bool MOS6551::getValue(string name, string& value)
{
    if (name == "dataRegister")
        value = getHexString(dataRegister);
    else if (name == "statusRegister")
        value = getHexString(statusRegister);
    else if (name == "commandRegister")
        value = getString(commandRegister);
    else if (name == "controlRegister")
        value = getString(controlRegister);
    else
        return false;
    
    return true;
}

OEChar MOS6551::read(OEAddress address)
{
    switch (address & 0x3)
    {
        case 0x0:
            return dataRegister;
            
        case 0x1:
            return statusRegister;
            
        case 0x2:
            return commandRegister;
            
        case 0x3:
            return controlRegister;
    }
    
    return 0;
}

void MOS6551::write(OEAddress address, OEChar value)
{
    switch (address & 0x3)
    {
        case 0x0:
            dataRegister = value;
            
            break;
            
        case 0x1:
            statusRegister = value;
            
            break;
            
        case 0x2:
            commandRegister = value;
            
            break;
            
        case 0x3:
            controlRegister = value;
            
            break;
    }
}

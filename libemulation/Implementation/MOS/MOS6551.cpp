
/**
 * libemulation
 * MOS6551
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6551 ACIA
 */

#include "MOS6551.h"

#include "ControlBusInterface.h"

MOS6551::MOS6551()
{
    initACIA();
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

bool MOS6551::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
    }
    else
        return false;
    
    return true;
}

void MOS6551::notify(OEComponent *sender, int notification, void *data)
{
    initACIA();
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
            break;
            
        case 0x2:
            commandRegister = value;
            
            break;
            
        case 0x3:
            controlRegister = value;
            
            break;
    }
}

void MOS6551::initACIA()
{
    dataRegister = 0;
    statusRegister = 0x10;
    commandRegister = 0;
    controlRegister = 0;
}
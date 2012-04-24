
/**
 * libemulation
 * MOS6530
 * (C) 2009-2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6530 (ROM, RAM, I/O, Timer)
 */

#include "MOS6530.h"

#include "ControlBusInterface.h"
#include "AddressDecoder.h"

MOS6530::MOS6530()
{
    controlBus = NULL;
    portA = NULL;
    portB = NULL;
    controlBusB = NULL;
}

bool MOS6530::setValue(string name, string value)
{
    if (name == "directionA")
        directionA = getOEInt(value);
    else if (name == "dataA")
        dataA = getOEInt(value);
    else if (name == "directionB")
        directionB = getOEInt(value);
    else if (name == "dataB")
        dataB = getOEInt(value);
    else
        return false;
    
    return true;
}

bool MOS6530::getValue(string name, string& value)
{
    if (name == "directionA")
        value = getHexString(directionA);
    else if (name == "dataA")
        value = getHexString(dataA);
    else if (name == "directionB")
        value = getHexString(directionB);
    else if (name == "dataB")
        value = getHexString(dataB);
    else
        return false;
    
    return true;
}

bool MOS6530::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
    {
        removeObserver(controlBus, CONTROLBUS_RESET_DID_ASSERT);
        controlBus = ref;
        addObserver(controlBus, CONTROLBUS_RESET_DID_ASSERT);
    }
    else if (name == "portA")
        portA = ref;
    else if (name == "portB")
        portB = ref;
    else if (name == "controlBusB")
        controlBusB = ref;
    else
        return false;
    
    return true;
}

OEChar MOS6530::read(OEAddress address)
{
    switch (address & 0xf)
    {
        case 0x0:
        case 0x4:
            dataA &= directionA;
            if (portA)
                dataA |= portA->read(0) & ~directionA;
            return dataA;
        case 0x1:
        case 0x5:
            return directionA;
        case 0x2:
        case 0x6:
            dataB &= directionB;
            if (portB)
                dataB |= portB->read(1) & ~directionB;
            return dataB;
        case 0x3:
        case 0x7:
            return directionB;
        case 0x8:
        case 0x9:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0xf:
            return 0;
    }
    
    return 0;
}

void MOS6530::write(OEAddress address, OEChar value)
{
    switch (address & 0xf)
    {
        case 0x0:
        case 0x4:
            dataA = value;
            if (portA)
                portA->write(0, value);
            break;
        case 0x1:
        case 0x5:
            directionA = value;
            break;
        case 0x2:
        case 0x6:
            dataB = value;
            if (portB)
                portB->write(1, value);
            break;
        case 0x3:
        case 0x7:
            directionB = value;
            break;
        case 0x8:
        case 0x9:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0xf:
            break;
    }
}

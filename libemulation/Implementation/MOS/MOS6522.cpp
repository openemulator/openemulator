
/**
 * libemulation
 * MOS6522
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6530 (ROM, RAM, I/O, Timer)
 */

#include "MOS6522.h"

#include "ControlBusInterface.h"
#include "AddressDecoder.h"

MOS6522::MOS6522()
{
    controlBus = NULL;
    portA = NULL;
    portB = NULL;
    controlBusB = NULL;
}

bool MOS6522::setValue(string name, string value)
{
    if (name == "ddrA")
        ddrA = getOEInt(value);
    else if (name == "dataA")
        dataA = getOEInt(value);
    else if (name == "ddrB")
        ddrB = getOEInt(value);
    else if (name == "dataB")
        dataB = getOEInt(value);
    else
        return false;
    
    return true;
}

bool MOS6522::getValue(string name, string& value)
{
    if (name == "ddrA")
        value = getHexString(ddrA);
    else if (name == "dataA")
        value = getHexString(dataA);
    else if (name == "ddrB")
        value = getHexString(ddrB);
    else if (name == "dataB")
        value = getHexString(dataB);
    else
        return false;
    
    return true;
}

bool MOS6522::setRef(string name, OEComponent *ref)
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

OEChar MOS6522::read(OEAddress address)
{
    switch (address & 0xf)
    {
        case 0x0:
        case 0x4:
            dataA &= ddrA;
            if (portA)
                dataA |= portA->read(0) & ~ddrA;
            return dataA;
        case 0x1:
        case 0x5:
            return ddrA;
        case 0x2:
        case 0x6:
            dataB &= ddrB;
            if (portB)
                dataB |= portB->read(1) & ~ddrB;
            return dataB;
        case 0x3:
        case 0x7:
            return ddrB;
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

void MOS6522::write(OEAddress address, OEChar value)
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
            ddrA = value;
            break;
        case 0x2:
        case 0x6:
            dataB = value;
            if (portB)
                portB->write(1, value);
            break;
        case 0x3:
        case 0x7:
            ddrB = value;
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


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
    
    addressA = 0;
    ddrA = 0;
    dataA = 0;
    ca1 = false;
    ca2 = false;
    
    addressB = 1;
    ddrB = 0;
    dataB = 0;
    cb1 = false;
    cb2 = false;
}

bool MOS6522::setValue(string name, string value)
{
    if (name == "addressA")
        addressA = getOELong(value);
    else if (name == "ddrA")
        ddrA = getOEInt(value);
    else if (name == "dataA")
        dataA = getOEInt(value);
    else if (name == "ca1")
        ca1 = getOEInt(value);
    else if (name == "ca2")
        ca2 = getOEInt(value);
    else if (name == "addressB")
        addressB = getOELong(value);
    else if (name == "ddrB")
        ddrB = getOEInt(value);
    else if (name == "dataB")
        dataB = getOEInt(value);
    else if (name == "cb1")
        cb1 = getOEInt(value);
    else if (name == "cb2")
        cb2 = getOEInt(value);
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
    else if (name == "ca1")
        value = getString(ca1);
    else if (name == "ca2")
        value = getString(ca2);
    else if (name == "ddrB")
        value = getHexString(ddrB);
    else if (name == "dataB")
        value = getHexString(dataB);
    else if (name == "cb1")
        value = getString(cb1);
    else if (name == "cb2")
        value = getString(cb2);
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

bool MOS6522::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case MOS6522_GET_PA:
            *((OEChar *)data) = dataA;
            
            return true;
            
        case MOS6522_GET_PB:
            *((OEChar *)data) = dataB;
            
            return true;
    }
    
    return false;
}

OEChar MOS6522::read(OEAddress address)
{
    switch (address & 0xf)
    {
        case 0x0:
            dataB &= ddrB;
            if (portB)
                dataB |= portB->read(addressB) & ~ddrB;
            
            return dataB;
            
        case 0x1:
            dataA &= ddrA;
            if (portA)
                dataA |= portA->read(addressA) & ~ddrA;
            
            return dataA;
            
        case 0x2:
            return ddrB;
            
        case 0x3:
            return ddrA;
            
        case 0xd:
            return 0xff;
            
        case 0xf:
            dataA &= ddrA;
            if (portA)
                dataA |= portA->read(addressA) & ~ddrA;
            
            return dataA;
    }
    
    return 0;
}

void MOS6522::write(OEAddress address, OEChar value)
{
    switch (address & 0xf)
    {
        case 0x0:
            dataB = value;
            
            if (portB)
                portB->write(addressB, value);
            
            break;
            
        case 0x1:
            dataA = value;
            
            if (portA)
                portA->write(addressA, value);
            
            break;
            
        case 0x2:
            ddrB = value;
            
            break;
            
        case 0x3:
            ddrA = value;
            
            break;
            
        case 0xf:
            dataA = value;
            
            if (portA)
                portA->write(addressA, value);
            
            break;
    }
}

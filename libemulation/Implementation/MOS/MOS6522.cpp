
/**
 * libemulation
 * MOS6522
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MOS 6522 VIA (Versatile Interface Adaptor)
 */

#include "MOS6522.h"

#include "ControlBusInterface.h"
#include "AddressDecoder.h"

#define RS_DATA_B               0x00
#define RS_DATA_A               0x01
#define RS_DDR_B                0x02
#define RS_DDR_A                0x03
#define RS_TIMER1_COUNTL        0x04
#define RS_TIMER1_COUNTH        0x05
#define RS_TIMER1_LATCHL        0x06
#define RS_TIMER1_LATCHH        0x07
#define RS_TIMER2_COUNTL        0x08
#define RS_TIMER2_COUNTH        0x09
#define RS_SHIFT                0x0a
#define RS_AUXCONTROL           0x0b
#define RS_PERIPHERALCONTROL    0x0c
#define RS_INTERRUPTFLAGS       0x0d
#define RS_INTERRUPTENABLE      0x0e
#define RS_DATA_A_NO_HS         0x0f

#define INT_CA2         (1 << 0)
#define INT_CA1         (1 << 1)
#define INT_SR          (1 << 2)
#define INT_CB2         (1 << 3)
#define INT_CB1         (1 << 4)
#define INT_T2          (1 << 5)
#define INT_T1          (1 << 6)
#define INT_SET         (1 << 7)
#define INT_IRQ         (1 << 7)

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
    
    shift = 0;
    
    auxControl = 0;
    peripheralControl = 0;
    
    interruptFlags = 0;
    interruptEnable = 0;
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
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
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
            
        case MOS6522_SET_CB1:
            if (*((bool *)data))
                OEAssertBit(interruptFlags, INT_CB1);
            
            updateIRQ();
            
            return true;
            
        case MOS6522_SET_CB2:
            if (*((bool *)data))
                OEAssertBit(interruptFlags, INT_CB2);
            
            updateIRQ();
            
            return true;
            
        case MOS6522_GET_PB:
            *((OEChar *)data) = dataB;
            
            return true;
    }
    
    return false;
}

void MOS6522::notify(OEComponent *sender, int notification, void *data)
{
    switch (notification)
    {
        case CONTROLBUS_RESET_DID_ASSERT:
            ddrA = 0;
            dataA = 0;
            ca1 = false;
            ca2 = false;
            portA->write(addressA, 0xff);
            
            ddrB = 0;
            dataB = 0;
            cb1 = false;
            cb2 = false;
            portB->write(addressB, 0xff);
            
            interruptFlags = 0;
            
            updateIRQ();
            
            break;
    }
}

OEChar MOS6522::read(OEAddress address)
{
    switch (address & 0xf)
    {
        case RS_DATA_B:
            OEClearBit(interruptFlags, INT_CB1 | INT_CB2);
            
            updateIRQ();
            
            return (dataB & ddrB) | (portB->read(addressB) & ~ddrB);
            
        case RS_DATA_A:
            OEClearBit(interruptFlags, INT_CA1 | INT_CA2);
            
            updateIRQ();
            
            return (dataA & ddrA) | (portA->read(addressA) & ~ddrA);
            
        case RS_DDR_B:
            return ddrB;
            
        case RS_DDR_A:
            return ddrA;
            
        case RS_SHIFT:
            return shift;
            
        case RS_AUXCONTROL:
            return auxControl;
            
        case RS_PERIPHERALCONTROL:
            return peripheralControl;
            
        case RS_INTERRUPTFLAGS:
//            logMessage("R " + getHexString(address) + ": " + getHexString(interruptFlags));
            
            return interruptFlags;
            
        case RS_INTERRUPTENABLE:
//            logMessage("R " + getHexString(address) + ": " + getHexString(interruptEnable));
            
            return interruptEnable;
            
        case RS_DATA_A_NO_HS:
            return (dataA & ddrA) | (portA->read(addressA) & ~ddrA);
    }
    
    return 0;
}

void MOS6522::write(OEAddress address, OEChar value)
{
/*    if (((address & 0xf) > 1) &&
        ((address & 0xf) < 15))
        logMessage("W " + getHexString(address) + ": " + getHexString(value));*/
    
    switch (address & 0xf)
    {
        case RS_DATA_B:
            OEClearBit(interruptFlags, INT_CB1 | INT_CB2);
            
            updateIRQ();
            
            dataB = value;
            
            if (portB)
                portB->write(addressB, (dataB & ddrB) | (~ddrB));
            
            break;
            
        case RS_DATA_A:
            OEClearBit(interruptFlags, INT_CA1 | INT_CA2);
            
            updateIRQ();
            
            dataA = value;
            
            if (portA)
                portA->write(addressA, (dataA & ddrA) | (~ddrA));
            
            break;
            
        case RS_DDR_B:
            ddrB = value;
            
            if (portB)
                portB->write(addressB, (dataB & ddrB) | (~ddrB));
            
            break;
            
        case RS_DDR_A:
            ddrA = value;
            
            if (portA)
                portA->write(addressA, (dataA & ddrA) | (~ddrA));
            
            break;
            
        case RS_SHIFT:
            shift = value;
            
            break;
            
        case RS_AUXCONTROL:
            auxControl = value;
            
            break;
            
        case RS_PERIPHERALCONTROL:
            peripheralControl = value;
            
            break;
            
        case RS_INTERRUPTFLAGS:
            OEClearBit(interruptFlags, value & 0x7f);
            
            updateIRQ();
            
            break;
            
        case RS_INTERRUPTENABLE:
            OESetBit(interruptEnable, value & 0x7f, OEGetBit(value, INT_SET));
            
            updateIRQ();
            
            break;
            
        case RS_DATA_A_NO_HS:
            dataA = value;
            
            if (portA)
                portA->write(addressA, (dataA & ddrA) | (~ddrA));
            
            break;
    }
}

void MOS6522::updateIRQ()
{
    bool wasIRQ = OEGetBit(interruptFlags, INT_IRQ);
    bool irq = interruptEnable & interruptFlags & 0x7f;
    
    OESetBit(interruptFlags, INT_IRQ, irq);
    
/*    if (!wasIRQ && irq)
        controlBus->postMessage(this, CONTROLBUS_ASSERT_IRQ, NULL);
    else if (wasIRQ && !irq)
        controlBus->postMessage(this, CONTROLBUS_CLEAR_IRQ, NULL);*/
}

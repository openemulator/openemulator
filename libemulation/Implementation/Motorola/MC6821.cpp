
/**
 * libemulation
 * MC6821
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an MC6821 PIA (Peripheral Interface Adapter)
 */

#include "MC6821.h"

#include "ControlBusInterface.h"
#include "AddressDecoder.h"

#define RS_DATA_A       0x00
#define RS_CONTROL_A    0x01
#define RS_DATA_B       0x02
#define RS_CONTROL_B    0x03

MC6821::MC6821()
{
    controlBus = NULL;
    portA = NULL;
    controlBusA = NULL;
    portB = NULL;
    controlBusB = NULL;
    
    addressA = 0;
    controlA = 0;
    ddrA = 0;
    dataA = 0;
    ca1 = false;
    ca2 = false;
    
    addressB = 1;
    controlB = 0;
    ddrB = 0;
    dataB = 0;
    cb1 = false;
    cb2 = false;
}

bool MC6821::setValue(string name, string value)
{
    if (name == "addressA")
        addressA = getOELong(value);
    else if (name == "controlA")
        setControlA(getOEInt(value));
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
    else if (name == "controlB")
        setControlB(getOEInt(value));
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

bool MC6821::getValue(string name, string& value)
{
    if (name == "controlA")
        value = getHexString(controlA);
    else if (name == "ddrA")
        value = getHexString(ddrA);
    else if (name == "dataA")
        value = getHexString(dataA);
    else if (name == "ca1")
        value = getString(ca1);
    else if (name == "ca2")
        value = getString(ca2);
    else if (name == "controlB")
        value = getHexString(controlB);
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

bool MC6821::setRef(string name, OEComponent *ref)
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
    else if (name == "controlBusA")
        controlBusA = ref;
    else if (name == "portB")
        portB = ref;
    else if (name == "controlBusB")
        controlBusB = ref;
    else
        return false;
    
    return true;
}

bool MC6821::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case MC6821_SET_CA1:
        {
            bool value = *((bool *) data);
            bool isLowToHigh = (controlA & MC6821_CR_C1LOWTOHIGH);
            
            if ((!isLowToHigh && ca1 && !value) ||
                (isLowToHigh && !ca1 && value))
            {
                if (controlA & MC6821_CR_C1ENABLEIRQ)
                    setControlA(controlA | MC6821_CR_IRQ1FLAG);
                
                if ((controlA & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT | MC6821_CR_C2ERESTORE)) ==
                    MC6821_CR_C2OUTPUT)
                    setCA2(true);
            }
            
            ca1 = value;
            
            return true;
        }
            
        case MC6821_SET_CA2:
        {
            if (controlA & MC6821_CR_C2OUTPUT)
                return false;
            
            bool value = *((bool *) data);
            
            if (controlA & MC6821_CR_C2ENABLEIRQ)
            {
                bool isLowToHigh = (controlA & MC6821_CR_C2LOWTOHIGH);
                
                if ((!isLowToHigh && ca2 && !value) ||
                    (isLowToHigh && !ca2 && value))
                    setControlA(controlA | MC6821_CR_IRQ2FLAG);
            }
            
            ca2 = value;
            
            return true;
        }
            
        case MC6821_GET_CA2:
        {
            if (!(controlA & MC6821_CR_C2OUTPUT))
                return false;
            
            *((bool *) data) = ca2;
            
            return true;
        }
            
        case MC6821_GET_PA:
            *((OEChar *) data) = dataA;
            return true;
            
        case MC6821_SET_CB1:
        {
            bool value = *((bool *) data);
            bool isLowToHigh = (controlB & MC6821_CR_C1LOWTOHIGH);
            
            if ((!isLowToHigh && cb1 && !value) ||
                (isLowToHigh && !cb1 && value))
            {
                if (controlB & MC6821_CR_C1ENABLEIRQ)
                    setControlB(controlB | MC6821_CR_IRQ1FLAG);
                
                if ((controlB & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT | MC6821_CR_C2ERESTORE)) ==
                    MC6821_CR_C2OUTPUT)
                    setCB2(true);
            }
            
            cb1 = value;
            
            return true;
        }
            
        case MC6821_SET_CB2:
        {
            if (controlB & MC6821_CR_C2OUTPUT)
                return false;
            
            bool value = *((bool *) data);
            
            if (controlB & MC6821_CR_C2ENABLEIRQ)
            {
                bool isLowToHigh = (controlB & MC6821_CR_C2LOWTOHIGH);
                
                if ((!isLowToHigh && cb2 && !value) ||
                    (isLowToHigh && !cb2 && value))
                    setControlB(controlB | MC6821_CR_IRQ2FLAG);
            }
            
            cb2 = value;
            
            return true;
        }
            
        case MC6821_GET_CB2:
        {
            if (!(controlB & MC6821_CR_C2OUTPUT))
                return false;
            
            *((bool *) data) = cb2;
            
            return true;
        }
            
        case MC6821_GET_PB:
            *((OEChar *) data) = dataB;
            return true;
    }
    
    return false;
}

void MC6821::notify(OEComponent *component, int notification, void *data)
{
    switch (notification)
    {
        case CONTROLBUS_RESET_DID_ASSERT:
            setControlA(0);
            ddrA = 0;
            dataA = 0;
            ca1 = false;
            ca2 = false;
            
            setControlB(0);
            ddrB = 0;
            dataB = 0;
            cb1 = false;
            cb2 = false;
            break;
    }
}

OEChar MC6821::read(OEAddress address)
{
    switch(address & 0x3)
    {
        case RS_DATA_A:
            if (controlA & MC6821_CR_DATAREGISTER)
            {
                setControlA(controlA & ~MC6821_CR_IRQFLAGS);
                dataA &= ddrA;
                dataA |= portA->read(addressA) & ~ddrA;
                
                if ((controlA & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)) ==
                    MC6821_CR_C2OUTPUT)
                {
                    setCA2(false);
                    
                    if (controlA & MC6821_CR_C2ERESTORE)
                        setCA2(true);
                }
                
                return dataA;
            }
            else
                return ddrA;
            
        case RS_CONTROL_A:
            return controlA;
            
        case RS_DATA_B:
            if (controlB & MC6821_CR_DATAREGISTER)
            {
                setControlB(controlB & ~MC6821_CR_IRQFLAGS);
                dataB &= ddrB;
                dataB |= portB->read(addressB) & ~ddrB;
                
                return dataB;
            }
            else
                return ddrB;
            
        case RS_CONTROL_B:
            return controlB;
    }
    
    return 0;
}

void MC6821::write(OEAddress address, OEChar value)
{
    switch(address & 0x3)
    {
        case RS_DATA_A:
            if (controlA & MC6821_CR_DATAREGISTER)
            {
                dataA = value & ddrA;
                portA->write(addressA, dataA);
            }
            else
                ddrA = value;
            
            break;
            
        case RS_CONTROL_A:
            setControlA(value);
            
            if ((value & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)) ==
                (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT))
                setCA2(value & MC6821_CR_C2SET);
            
            break;
            
        case RS_DATA_B:
            if (controlB & MC6821_CR_DATAREGISTER)
            {
                dataB = value & ddrB;
                portB->write(addressB, dataB);
                
                if ((controlB & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)) ==
                    MC6821_CR_C2OUTPUT)
                {
                    setCB2(false);
                    
                    if (controlB & MC6821_CR_C2ERESTORE)
                        setCB2(true);
                }
            }
            else
                ddrB = value;
            
            break;
            
        case RS_CONTROL_B:
            setControlB(value);
            
            if ((value & (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT)) ==
                (MC6821_CR_C2OUTPUT | MC6821_CR_C2DIRECT))
                setCB2(value & MC6821_CR_C2SET);
            
            break;
    }
}

void MC6821::setCA2(bool value)
{
    if (ca2 != value)
        postNotification(this, MC6821_CA2_DID_CHANGE, &value);
    
    ca2 = value;
}

void MC6821::setCB2(bool value)
{
    if (cb2 != value)
        postNotification(this, MC6821_CB2_DID_CHANGE, &value);
    
    cb2 = value;
}

void MC6821::setControlA(OEChar value)
{
    bool wasIRQFlag = controlA & MC6821_CR_IRQFLAGS;
    controlA = value;
    bool isIRQFlag = controlA & MC6821_CR_IRQFLAGS;
    
    if (controlBusA)
    {
        if (wasIRQFlag && !isIRQFlag)
            controlBusA->postMessage(this, CONTROLBUS_ASSERT_IRQ, NULL);
        else if (!wasIRQFlag && isIRQFlag)
            controlBusA->postMessage(this, CONTROLBUS_CLEAR_IRQ, NULL);
    }
}

void MC6821::setControlB(OEChar value)
{
    bool wasIRQFlag = controlB & MC6821_CR_IRQFLAGS;
    controlB = value;
    bool isIRQFlag = controlB & MC6821_CR_IRQFLAGS;
    
    if (controlBusB)
    {
        if (wasIRQFlag && !isIRQFlag)
            controlBusB->postMessage(this, CONTROLBUS_ASSERT_IRQ, NULL);
        else if (!wasIRQFlag && isIRQFlag)
            controlBusB->postMessage(this, CONTROLBUS_CLEAR_IRQ, NULL);
    }
}

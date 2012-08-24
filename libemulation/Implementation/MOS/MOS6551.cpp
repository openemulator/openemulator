
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

#include "RS232Interface.h"

#define CR_RECEIVER_EN          (1 << 0)
#define CR_RECEIVE_INT          (1 << 1)
#define CR_BREAK                (1 << 3)
#define CR_TRANSMIT_INT         (1 << 2)    // If CR_BREAK clear
#define CR_TRANSMIT_BREAK       (1 << 2)    // If CR_BREAK set
#define CR_ECHO                 (1 << 4)
#define CR_PARITY               (1 << 5)
#define CR_EVEN_PARITY          (1 << 6)
#define CR_NOT_RECEIVE_PARITY   (1 << 7)

#define SR_PARITYERROR          (1 << 0)
#define SR_FRAMEERROR           (1 << 1)
#define SR_OVERRUN              (1 << 2)
#define SR_RECEIVE_FULL         (1 << 3)
#define SR_TRANSMIT_EMPTY       (1 << 4)
#define SR_NOT_DCD              (1 << 5)
#define SR_NOT_DSR              (1 << 6)
#define SR_INT                  (1 << 7)

MOS6551::MOS6551()
{
    hardReset();
}

bool MOS6551::setValue(string name, string value)
{
    if (name == "controlRegister")
        control = getOEInt(value);
    else if (name == "commandRegister")
        command = getOEInt(value);
    else if (name == "statusRegister")
        status = getOEInt(value);
    else
        return false;
    
    return true;
}

bool MOS6551::getValue(string name, string& value)
{
    if (name == "controlRegister")
        value = getString(control);
    else if (name == "commandRegister")
        value = getString(command);
    else if (name == "statusRegister")
        value = getHexString(status);
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
    else if (name == "dce")
    {
        if (dce)
        {
            cts = true;
            OESetBit(status, SR_NOT_DSR, false);
            OESetBit(status, SR_NOT_DCD, true);
        }
        dce = ref;
        if (dce)
        {
            bool value;
            
            dce->postMessage(this, RS232_IS_CTS, &cts);
            
            dce->postMessage(this, RS232_IS_DSR, &value);
            OESetBit(status, SR_NOT_DSR, !value);
            
            dce->postMessage(this, RS232_IS_DCD, &value);
            OESetBit(status, SR_NOT_DCD, !value);
        }
    }
    else
        return false;
    
    return true;
}

bool MOS6551::init()
{
    OECheckComponent(controlBus);
    
    return true;
}

bool MOS6551::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case RS232_RECEIVE_DATA:
            // Is the receiver turned on?
            if (!OEGetBit(command, CR_RECEIVER_EN))
                return false;
            
            // Do we have DCD?
            if (OEGetBit(status, SR_NOT_DCD))
                return false;
            
            receiveData = *((OEChar *)data);
            
            OEAssertBit(status, SR_RECEIVE_FULL);
            
            if (!isTransmitInt())
                setIRQ(true);
            
            return true;
            
        case RS232_IS_RTS:
            *((bool *)data) = isRTS();
            
            return true;
            
        case RS232_SET_CTS:
            cts = *((bool *)data);
            
            return true;
            
        case RS232_IS_DTR:
            *((bool *)data) = OEGetBit(command, CR_RECEIVER_EN);
            
            return true;
            
        case RS232_SET_DSR:
            OESetBit(status, SR_NOT_DSR, !*((bool *)data));
            
            setIRQ(true);
            
            return true;
            
        case RS232_SET_DCD:
            OESetBit(status, SR_NOT_DCD, !*((bool *)data));
            
            setIRQ(true);
            
            return true;
    }
    
    return false;
}

void MOS6551::notify(OEComponent *sender, int notification, void *data)
{
    hardReset();
}

OEChar MOS6551::read(OEAddress address)
{
    switch (address & 0x3)
    {
        case 0x0:
            OEClearBit(status, SR_RECEIVE_FULL);
            
            setIRQ(false);
            
            return receiveData;
            
        case 0x1:
        {
            OEChar oldStatus = status;
            
            if (!isTransmitInt())
                setIRQ(false);
            
            return oldStatus;
        }
        case 0x2:
            return command;
            
        case 0x3:
            return control;
    }
    
    return 0;
}

void MOS6551::write(OEAddress address, OEChar value)
{
    switch (address & 0x3)
    {
        case 0x0:
            OEClearBit(status, SR_TRANSMIT_EMPTY);
            
            transmitData = value;
            
            setIRQ(false);
            
            sendData();
            
            break;
            
        case 0x1:
            softReset();
            
            break;
            
        case 0x2:
            setCommand(value);
            
            break;
            
        case 0x3:
            control = value;
            
            break;
    }
}

void MOS6551::hardReset()
{
    transmitData = 0;
    receiveData = 0;
    
    control = 0;
    command = 0;
    OEClearBit(status, (SR_RECEIVE_FULL |
                        SR_OVERRUN |
                        SR_FRAMEERROR |
                        SR_PARITYERROR));
    OEAssertBit(status, SR_TRANSMIT_EMPTY);
    
    setIRQ(false);
}

void MOS6551::softReset()
{
    OEClearBit(command, (CR_ECHO |
                         CR_BREAK |
                         CR_TRANSMIT_BREAK |
                         CR_RECEIVE_INT |
                         CR_RECEIVER_EN));
    
    OEClearBit(status, SR_OVERRUN);
    
    setIRQ(false);
}

void MOS6551::sendData()
{
    // Check if it is a good time to send
    if (cts && !OEGetBit(status, SR_TRANSMIT_EMPTY))
    {
        if (dce)
            dce->postMessage(this, RS232_TRANSMIT_DATA, &transmitData);
        
        OEAssertBit(status, SR_TRANSMIT_EMPTY);
        
        if (isTransmitInt())
            setIRQ(true);
    }
}

void MOS6551::setCommand(OEChar value)
{
    bool wasDTR = OEGetBit(command, CR_RECEIVER_EN);
    bool isDTR = OEGetBit(value, CR_RECEIVER_EN);
    
    bool wasRTS = !(command & (CR_BREAK | CR_TRANSMIT_INT));
    bool isRTS = !(value & (CR_BREAK | CR_TRANSMIT_INT));
    
    command = value;
    
    if (dce)
    {
        if (wasDTR != isDTR)
            dce->postMessage(this, RS232_SET_DTR, &isDTR);
        
        if (wasRTS != isRTS)
            dce->postMessage(this, RS232_SET_RTS, &isRTS);
    }
    
    if (!OEGetBit(command, CR_RECEIVER_EN))
        setIRQ(false);
}

void MOS6551::setIRQ(bool value)
{
    // IRQ's are enabled when receiver is enabled
    bool wasIRQFlag = OEGetBit(status, SR_INT);
    bool isIRQFlag = OEGetBit(command, CR_RECEIVER_EN) & value;
    
    OESetBit(status, SR_INT, isIRQFlag);
    
    if (!wasIRQFlag && isIRQFlag)
        controlBus->postMessage(this, CONTROLBUS_ASSERT_IRQ, NULL);
    else if (wasIRQFlag && !isIRQFlag)
        controlBus->postMessage(this, CONTROLBUS_CLEAR_IRQ, NULL);
}

bool MOS6551::isTransmitInt()
{
    return ((command & (CR_BREAK | CR_TRANSMIT_INT)) == CR_TRANSMIT_INT);
}

bool MOS6551::isRTS()
{
    return (command & (CR_BREAK | CR_TRANSMIT_INT)) == 0;
}

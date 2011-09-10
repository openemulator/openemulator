
/**
 * libemulation
 * Apple I input/output
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls Apple I input/output PIA memory
 */

#include "Apple1IO.h"

#include "CanvasInterface.h"
#include "RS232Interface.h"
#include "MC6821.h"

#define APPLE1KEYBOARD_MASK	0x80

Apple1IO::Apple1IO()
{
    terminal = NULL;
    
    enhancedTerminalSpeed = false;
    fullASCIIKeyboard = false;
}

bool Apple1IO::setValue(string name, string value)
{
    if (name == "terminalSpeed")
        enhancedTerminalSpeed = (value == "Enhanced");
    else if (name == "keyboardType")
        fullASCIIKeyboard = (value == "Full ASCII");
    else
        return false;
    
    return true;
}

bool Apple1IO::getValue(string name, string &value)
{
    if (name == "terminalSpeed")
        value = enhancedTerminalSpeed ? "Enhanced" : "Standard";
    else if (name == "keyboardType")
        value = fullASCIIKeyboard ? "Full ASCII" : "Standard";
    else
        return false;
    
    return true;
}

bool Apple1IO::setRef(string name, OEComponent *ref)
{
    if (name == "pia")
    {
        if (pia)
            pia->removeObserver(this, MC6821_CB2_DID_CHANGE);
        pia = ref;
        if (pia)
            pia->addObserver(this, MC6821_CB2_DID_CHANGE);
    }
    else if (name == "terminal")
    {
        if (terminal)
        {
            terminal->removeObserver(this, RS232_DID_RECEIVE_DATA);
            terminal->removeObserver(this, RS232_CTS_DID_ASSERT);
        }
        terminal = ref;
        if (terminal)
        {
            terminal->addObserver(this, RS232_DID_RECEIVE_DATA);
            terminal->addObserver(this, RS232_CTS_DID_ASSERT);
        }
    }
    else
        return false;
    
    return true;
}

void Apple1IO::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == terminal)
    {
        switch (notification)
        {
            case RS232_DID_RECEIVE_DATA:
                {
                    OEData *s = (OEData *)data;
                    termKey = s->at(0);
                }
                return;
                
            case RS232_CTS_DID_ASSERT:
                {
                    OEData data;
                    data.push_back(termChar);
                    terminal->postMessage(this, RS232_SEND_DATA, &data);
                }
                return;
        }
    }
    else if (sender == pia)
    {
        // CB2 did change
        
    }
}

OEUInt8 Apple1IO::read(OEAddress address)
{
    switch (address & 0)
    {
        case 0:
            return termKey | APPLE1KEYBOARD_MASK;
            
        case 1:
            {
                bool cb2;
                terminal->postMessage(this, MC6821_GET_CB2, &cb2);
                return ((!cb2) << 7);
            }
    }
    
    return 0;
}

void Apple1IO::write(OEAddress address, OEUInt8 value)
{
    switch (address & 0)
    {
        case 0:
            return;
            
        case 1:
            termChar = value & 0x7f;
            return;
    }
}

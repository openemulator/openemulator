
/**
 * libemulation
 * Apple-1 input/output
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Interfaces an Apple-1 terminal with a PIA
 */

#include "Apple1IO.h"

#include "MC6821.h"

#include "CanvasInterface.h"
#include "RS232Interface.h"

#define APPLE1KEYBOARD_MASK	0x80

Apple1IO::Apple1IO()
{
    enhancedTerminalSpeed = false;
    fullASCIIKeyboard = false;
    
    pia = NULL;
    terminal = NULL;
    
    terminalKey = 0;
    terminalChar = 0;
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

bool Apple1IO::getValue(string name, string& value)
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
            terminal->removeObserver(this, RS232_CTS_DID_CHANGE);
        }
        terminal = ref;
        if (terminal)
        {
            terminal->addObserver(this, RS232_DID_RECEIVE_DATA);
            terminal->addObserver(this, RS232_CTS_DID_CHANGE);
            
            terminal->postMessage(this, RS232_ASSERT_RTS, NULL);
        }
    }
    else
        return false;
    
    return true;
}

bool Apple1IO::init()
{
    OECheckComponent(pia);
    OECheckComponent(terminal);
    
    pia->postMessage(this, MC6821_GET_PB, &terminalChar);
    
    terminalChar &= 0x7f;
    
    sendChar();
    
    return true;
}

void Apple1IO::update()
{
    sendChar();
}

void Apple1IO::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == pia)
    {
        switch (notification)
        {
            case MC6821_CB2_DID_CHANGE:
            {
                bool ready = *((bool *)data);
                
                if (ready && !enhancedTerminalSpeed)
                    sendChar();
            }
        }
    }
    else if (sender == terminal)
    {
        switch (notification)
        {
            case RS232_DID_RECEIVE_DATA:
            {
                // Get key
                OEData *s = (OEData *)data;
                OEChar key = s->at(0);
                
                if (!fullASCIIKeyboard)
                {
                    if (key >= 'a' && key <= 'z')
                        key -= 0x20;
                    else if (key >= 0x60 && key <= 0x7f)
                        return;
                }
                
                terminalKey = key;
                
                // Signal Keyboard strobe
                bool ca1 = true;
                pia->postMessage(this, MC6821_SET_CA1, &ca1);
                ca1 = false;
                pia->postMessage(this, MC6821_SET_CA1, &ca1);
                
                terminal->postMessage(this, RS232_CLEAR_RTS, NULL);
                
                return;
            }
                
            case RS232_CTS_DID_CHANGE:
            {
                bool cts = *((bool *)data);
                
                if (cts)
                {
                    // Signal /RDA
                    bool cb1 = true;
                    pia->postMessage(this, MC6821_SET_CB1, &cb1);
                    // (it should last 3.5 µs, but we'll toggle a bit faster)
                    cb1 = false;
                    pia->postMessage(this, MC6821_SET_CB1, &cb1);
                }
                
                return;
            }
        }
    }
}

OEChar Apple1IO::read(OEAddress address)
{
    switch (address & 1)
    {
        case 0:
        {
            OEChar value = terminalKey | APPLE1KEYBOARD_MASK;
            
            terminalKey = 0;
            
            terminal->postMessage(this, RS232_ASSERT_RTS, NULL);
            
            return value;
        }
            
        case 1:
        {
            bool ready;
            
            if (enhancedTerminalSpeed)
                ready = true;
            else
                // Loop /CB2 signal to PB7
                pia->postMessage(this, MC6821_GET_CB2, &ready);
            
            return ((!ready) << 7);
        }
    }
    
    return 0;
}

void Apple1IO::write(OEAddress address, OEChar value)
{
    switch (address & 1)
    {
        case 0:
            break;
            
        case 1:
            terminalChar = value & 0x7f;
            
            if (enhancedTerminalSpeed)
                sendChar();
            
            break;
    }
}

void Apple1IO::sendChar()
{
    if (!terminalChar)
        return;
    
    OEData s;
    s.push_back(terminalChar);
    terminal->postMessage(this, RS232_SEND_DATA, &s);
    
    terminalChar = 0;
}

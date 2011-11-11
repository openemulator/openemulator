
/**
 * libemulator
 * Apple II Keyboard
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II keyboard
 */

#include "AppleIIKeyboard.h"

#include "ControlBusInterface.h"

AppleIIKeyboard::AppleIIKeyboard()
{
    type = APPLEIIKEYBOARD_TYPE_STANDARD;
    
    controlBus = NULL;
    floatingBus = NULL;
    gamePort = NULL;
    monitor = NULL;
}

bool AppleIIKeyboard::setValue(string name, string value)
{
	if (name == "type")
    {
        if (value == "Standard")
            type = APPLEIIKEYBOARD_TYPE_STANDARD;
        else if (value == "Shift-Key Mod")
            type = APPLEIIKEYBOARD_TYPE_SHIFTKEYMOD;
        else if (value == "Full ASCII")
            type = APPLEIIKEYBOARD_TYPE_FULLASCII;
    }
	else
		return false;
	
	return true;
}

bool AppleIIKeyboard::getValue(string name, string& value)
{
	if (name == "type")
    {
        if (type == APPLEIIKEYBOARD_TYPE_STANDARD)
            value = "Standard";
        else if (type == APPLEIIKEYBOARD_TYPE_SHIFTKEYMOD)
            value = "Shift-Key Mod";
        else if (type == APPLEIIKEYBOARD_TYPE_FULLASCII)
            value = "Full ASCII";
    }
	else
		return false;
	
	return true;
}

bool AppleIIKeyboard::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
    }
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "gamePort")
        gamePort = ref;
	else if (name == "monitor")
    {
        if (monitor)
        {
            monitor->removeObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
            monitor->removeObserver(this, CANVAS_DID_PASTE);
        }
        monitor = ref;
        if (monitor)
        {
            monitor->addObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
            monitor->addObserver(this, CANVAS_DID_PASTE);
        }
    }
	else
		return false;
	
	return true;
}

void AppleIIKeyboard::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_RESET_DID_ASSERT:
                // Clear pastebuffer
                while (!pasteBuffer.empty())
                    pasteBuffer.pop();
                
                break;
        }
    }
    else if (sender == monitor)
    {
        switch (notification)
        {
            case CANVAS_UNICODECHAR_WAS_SENT:
            {
                if (!pasteBuffer.empty())
                    break;
                
                sendKey(*((CanvasUnicodeChar *)data));
                
                break;
            }
                
            case CANVAS_DID_PASTE:
                paste((wstring *)data);
                
                break;
        }
    }
}

OEUInt8 AppleIIKeyboard::read(OEAddress address)
{
    if (address & 0x10)
    {
        keyLatch &= 0x7f;
        
        emptyPasteBuffer();
    }
    else
        return keyLatch;
    
	return floatingBus->read(address);
}

void AppleIIKeyboard::write(OEAddress address, OEUInt8 value)
{
    if (address & 0x10)
    {
        keyLatch &= 0x7f;
        
        emptyPasteBuffer();
    }
}

void AppleIIKeyboard::sendKey(CanvasUnicodeChar key)
{
    if (key >= 0x80)
        return;
    
    if (key == '\n')
        key = '\r';
    else if (key == '\r')
        return;
    
    if (type != APPLEIIKEYBOARD_TYPE_FULLASCII)
    {
        if (key >= 'a' && key <= 'z')
            key -= 0x20;
        else if (key >= 0x60 && key <= 0x7f)
            return;
    }
    
    keyLatch = key | 0x80;
}

void AppleIIKeyboard::paste(wstring *s)
{
    for (int i = 0; i < s->size(); i++)
        pasteBuffer.push(s->at(i));
    
    emptyPasteBuffer();
}

void AppleIIKeyboard::emptyPasteBuffer()
{
    while (!(keyLatch & 0x80) && !pasteBuffer.empty())
    {
        sendKey(pasteBuffer.front());
        
        pasteBuffer.pop();
    }
}

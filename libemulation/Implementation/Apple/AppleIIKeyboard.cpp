
/**
 * libemulator
 * Apple II Keyboard
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II keyboard
 */

#include "AppleIIKeyboard.h"

#include "DeviceInterface.h"
#include "ControlBusInterface.h"
#include "AppleIIInterface.h"

AppleIIKeyboard::AppleIIKeyboard()
{
    type = APPLEIIKEYBOARD_TYPE_STANDARD;
    
    controlBus = NULL;
    floatingBus = NULL;
    gamePort = NULL;
    monitor = NULL;
    
    keyLatch = 0;
    state = APPLEIIKEYBOARD_STATE_NORMAL;
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
        {
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        }
        controlBus = ref;
        if (controlBus)
        {
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
        }
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
            monitor->removeObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
            monitor->removeObserver(this, CANVAS_DID_PASTE);
        }
        monitor = ref;
        if (monitor)
        {
            monitor->addObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
            monitor->addObserver(this, CANVAS_KEYBOARD_DID_CHANGE);
            monitor->addObserver(this, CANVAS_DID_PASTE);
        }
    }
	else
		return false;
	
	return true;
}

bool AppleIIKeyboard::init()
{
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    update();
    
    return true;
}

void AppleIIKeyboard::update()
{
    updateShiftKeyMod();
}

void AppleIIKeyboard::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
                if (*((ControlBusPowerState *)data) == CONTROLBUS_POWERSTATE_ON)
                    keyLatch = 0;
                
                break;
                
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
                
            case CANVAS_KEYBOARD_DID_CHANGE:
            {
                CanvasHIDEvent *hidEvent = (CanvasHIDEvent *)data;
                
                switch (state)
                {
                    case APPLEIIKEYBOARD_STATE_NORMAL:
                        // Shift-key mod
                        updateShiftKeyMod();
                        
                        // React on key down
                        if (!hidEvent->value)
                            break;
                        else if (hidEvent->usageId == CANVAS_K_BACKSPACE)
                        {
                            CanvasKeyboardFlags flags;
                            
                            monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);
                            
                            if (OEGetBit(flags, CANVAS_KF_CONTROL) &&
                                OEGetBit(flags, CANVAS_KF_GUI))
                            {
                                ControlBusPowerState powerState = CONTROLBUS_POWERSTATE_OFF;
                                controlBus->postMessage(this, CONTROLBUS_SET_POWERSTATE, &powerState);
                                
                                state = APPLEIIKEYBOARD_STATE_RESTART;
                            }
                            else if (OEGetBit(flags, CANVAS_KF_CONTROL))
                            {
                                controlBus->postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
                                
                                state = APPLEIIKEYBOARD_STATE_RESET;
                            }
                        }
                        break;
                        
                    case APPLEIIKEYBOARD_STATE_RESET:
                    {
                        if (hidEvent->usageId == CANVAS_K_BACKSPACE)
                        {
                            controlBus->postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
                            
                            keyLatch = 0;
                            state = APPLEIIKEYBOARD_STATE_NORMAL;
                        }
                        
                        break;
                    }
                    case APPLEIIKEYBOARD_STATE_RESTART:
                    {
                        if (hidEvent->usageId == CANVAS_K_BACKSPACE)
                        {
                            ControlBusPowerState powerState = CONTROLBUS_POWERSTATE_ON;
                            controlBus->postMessage(this, CONTROLBUS_SET_POWERSTATE, &powerState);
                            
                            state = APPLEIIKEYBOARD_STATE_NORMAL;
                        }
                        
                        break;
                    }
                }
                
                break;
            }
                
            case CANVAS_DID_PASTE:
                paste((wstring *)data);
                
                break;
        }
    }
}

OEChar AppleIIKeyboard::read(OEAddress address)
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

void AppleIIKeyboard::write(OEAddress address, OEChar value)
{
    if (address & 0x10)
    {
        keyLatch &= 0x7f;
        
        emptyPasteBuffer();
    }
}

void AppleIIKeyboard::updateShiftKeyMod()
{
    if (type == APPLEIIKEYBOARD_TYPE_SHIFTKEYMOD)
    {
        if (!monitor || !gamePort)
            return;
        
        CanvasKeyboardFlags flags;
        
        monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);
        
        bool shiftKeyUp = !OEGetBit(flags, CANVAS_KF_SHIFT);
        
        gamePort->postMessage(this, APPLEII_SET_PB2, &shiftKeyUp);
    }
}

void AppleIIKeyboard::sendKey(CanvasUnicodeChar key)
{
    if (key == CANVAS_U_LEFT)
        key = 0x08;
    else if (key == CANVAS_U_RIGHT)
        key = 0x15;
    else if (key == 127)
        key = 8;
    else if (key >= 0x80)
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
    for (OEInt i = 0; i < s->size(); i++)
        pasteBuffer.push(s->at(i));
    
    emptyPasteBuffer();
}

void AppleIIKeyboard::emptyPasteBuffer()
{
    while (!(keyLatch & 0x80) && !pasteBuffer.empty())
    {
        OEInt c = pasteBuffer.front();
        
        if (c == '\n')
            c = '\r';
        else if (c == '\r')
            continue;
        
        sendKey(c);
        
        pasteBuffer.pop();
    }
}

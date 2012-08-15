
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
    keyStrobe = false;
    state = APPLEIIKEYBOARD_STATE_NORMAL;
    
    // Default value for Apple III key flags
    appleIIIKeyFlags = 0x7e;
    keypadCount = 0;
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
        else if (value == "Apple III")
            type = APPLEIIKEYBOARD_TYPE_APPLEIII;
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
        else if (type == APPLEIIKEYBOARD_TYPE_APPLEIII)
            value = "Apple III";
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
    OECheckComponent(controlBus);
    OECheckComponent(floatingBus);
    
    update();
    
    return true;
}

void AppleIIKeyboard::update()
{
    updateKeyFlags();
}

void AppleIIKeyboard::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
                if (*((ControlBusPowerState *)data) == CONTROLBUS_POWERSTATE_ON)
                {
                    keyLatch = 0;
                    keyStrobe = 0;
                }
                
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
                updateKeyFlags();
                
                CanvasHIDEvent *hidEvent = (CanvasHIDEvent *)data;
                
                if ((hidEvent->usageId >= CANVAS_KP_NUMLOCK) &&
                    (hidEvent->usageId <= CANVAS_KP_PERIOD))
                    keypadCount += hidEvent->value ? 1 : -1;
                
                switch (state)
                {
                    case APPLEIIKEYBOARD_STATE_NORMAL:
                        // React on key down
                        if (!hidEvent->value)
                            break;
                        
                        if (hidEvent->usageId == CANVAS_K_BACKSPACE)
                        {
                            stateUsageId = hidEvent->usageId;
                            
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
                        else if (hidEvent->usageId == CANVAS_K_F12)
                        {
                            stateUsageId = hidEvent->usageId;
                            
                            CanvasKeyboardFlags flags;
                            
                            monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);
                            
                            if (OEGetBit(flags, CANVAS_KF_CONTROL))
                            {
                                ControlBusPowerState powerState = CONTROLBUS_POWERSTATE_OFF;
                                controlBus->postMessage(this, CONTROLBUS_SET_POWERSTATE, &powerState);
                                
                                state = APPLEIIKEYBOARD_STATE_RESTART;
                            }
                            else
                            {
                                controlBus->postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
                                
                                state = APPLEIIKEYBOARD_STATE_RESET;
                            }
                        }
                        break;
                        
                    case APPLEIIKEYBOARD_STATE_RESET:
                        if (hidEvent->usageId == stateUsageId)
                        {
                            controlBus->postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
                            
                            state = APPLEIIKEYBOARD_STATE_NORMAL;
                        }
                        
                        break;
                        
                    case APPLEIIKEYBOARD_STATE_RESTART:
                        if (hidEvent->usageId == stateUsageId)
                        {
                            ControlBusPowerState powerState = CONTROLBUS_POWERSTATE_ON;
                            controlBus->postMessage(this, CONTROLBUS_SET_POWERSTATE, &powerState);
                            
                            state = APPLEIIKEYBOARD_STATE_NORMAL;
                        }
                        
                        break;
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
        keyStrobe = false;
        
        emptyPasteBuffer();
    }
    else
    {
        if ((type == APPLEIIKEYBOARD_TYPE_APPLEIII) &&
            (address & 0x08))
            return (keyLatch & 0x80) | appleIIIKeyFlags;
        
        return (keyStrobe << 7) | (keyLatch & 0x7f);
    }
    
	return floatingBus->read(address);
}

void AppleIIKeyboard::write(OEAddress address, OEChar value)
{
    if (address & 0x10)
    {
        keyStrobe = false;
        
        emptyPasteBuffer();
    }
}

void AppleIIKeyboard::updateKeyFlags()
{
    switch (type)
    {
        case APPLEIIKEYBOARD_TYPE_SHIFTKEYMOD:
        {
            if (!monitor || !gamePort)
                return;
            
            CanvasKeyboardFlags flags;
            
            monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);
            
            bool shiftKeyUp = !OEGetBit(flags, CANVAS_KF_SHIFT);
            
            gamePort->postMessage(this, APPLEII_SET_PB2, &shiftKeyUp);
            
            break;
        }
        case APPLEIIKEYBOARD_TYPE_APPLEIII:
        {
            if (monitor)
            {
                bool anyKeyDown;
                CanvasKeyboardFlags flags;
                
                monitor->postMessage(this, CANVAS_GET_KEYBOARD_ANYKEYDOWN, &anyKeyDown);
                monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);
                
                OESetBit(appleIIIKeyFlags, (1 << 0), anyKeyDown);
                OESetBit(appleIIIKeyFlags, (1 << 1), !OEGetBit(flags, CANVAS_KF_SHIFT));
                OESetBit(appleIIIKeyFlags, (1 << 2), !OEGetBit(flags, CANVAS_KF_CONTROL));
                OESetBit(appleIIIKeyFlags, (1 << 3), 1);   // Caps Lock
                OESetBit(appleIIIKeyFlags, (1 << 4), !OEGetBit(flags, CANVAS_KF_LEFTGUI));
                OESetBit(appleIIIKeyFlags, (1 << 5), !OEGetBit(flags, CANVAS_KF_RIGHTGUI));
                OESetBit(appleIIIKeyFlags, (1 << 6), 1);   // Keyboard connected
            }
        }
        default:
            break;
    }
}

void AppleIIKeyboard::sendKey(CanvasUnicodeChar key)
{
    if (type == APPLEIIKEYBOARD_TYPE_APPLEIII)
    {
        if (key == CANVAS_U_LEFT)
            key = 0x88;
        else if (key == CANVAS_U_RIGHT)
            key = 0x95;
        else if (key == CANVAS_U_UP)
            key = 0x8b;
        else if (key == CANVAS_U_DOWN)
            key = 0x8a;
        else if (key == 0x9)
            key = 0x88;
        else if (key == 0x1b)
            key = 0x98;
        else if (key == 0x20)
            key = 0xa0;
        else if (key == 0x7f)
            key = 0x88;
        else if (key >= 0x80)
            return;
        
        if (keypadCount)
            key |= 0x80;
        
        if (key >= 'a' && key <= 'z')
            key -= 0x20;
    }
    else
    {
        if (key == CANVAS_U_LEFT)
            key = 0x8;
        else if (key == CANVAS_U_RIGHT)
            key = 0x15;
        else if (key == 0x7f)
            key = 0x8;
        else if (key >= 0x80)
            return;
        
        if (type != APPLEIIKEYBOARD_TYPE_FULLASCII)
        {
            if (key >= 'a' && key <= 'z')
                key -= 0x20;
            else if (key >= 0x60 && key <= 0x7e)
                return;
        }
    }
    
    keyLatch = key;
    keyStrobe = true;
}

void AppleIIKeyboard::paste(wstring *s)
{
    for (OEInt i = 0; i < s->size(); i++)
        pasteBuffer.push(s->at(i));
    
    emptyPasteBuffer();
}

void AppleIIKeyboard::emptyPasteBuffer()
{
    while (!keyStrobe && !pasteBuffer.empty())
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

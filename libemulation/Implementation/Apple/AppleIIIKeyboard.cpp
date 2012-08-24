
/**
 * libemulation
 * Apple III Keyboard
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III keyboard
 */

#include "AppleIIIKeyboard.h"

#include "ControlBusInterface.h"

#include "AppleIIIInterface.h"

AppleIIIKeyboard::AppleIIIKeyboard()// : AppleIIKeyboard()
{
    appleIIIKeyFlags = (APPLEIII_NOT_SHIFT |
                        APPLEIII_NOT_CONTROL |
                        APPLEIII_NOT_CAPSLOCK |
                        APPLEIII_NOT_OPENAPPLE |
                        APPLEIII_NOT_CLOSEAPPLE |
                        APPLEIII_KEYBOARD_CONNECTED);
}

OEChar AppleIIIKeyboard::read(OEAddress address)
{
    if (address & 0x10)
    {
        setKeyStrobe(false);
        
        emptyPasteBuffer();
    }
    else
    {
        if (address & 0x08)
            return (keyLatch & 0x80) | appleIIIKeyFlags;
        
        return (keyStrobe << 7) | (keyLatch & 0x7f);
    }
    
	return floatingBus->read(address);
}

void AppleIIIKeyboard::updateKeyFlags()
{
    if (!monitor)
        return;
    
    bool anyKeyDown = false;
    CanvasKeyboardFlags flags = 0;
    
    monitor->postMessage(this, CANVAS_GET_KEYBOARD_ANYKEYDOWN, &anyKeyDown);
    monitor->postMessage(this, CANVAS_GET_KEYBOARD_FLAGS, &flags);
    
    OESetBit(appleIIIKeyFlags, APPLEIII_ANYKEYDOWN, anyKeyDown);
    OESetBit(appleIIIKeyFlags, APPLEIII_NOT_SHIFT, !OEGetBit(flags, CANVAS_KF_SHIFT));
    OESetBit(appleIIIKeyFlags, APPLEIII_NOT_CONTROL, !OEGetBit(flags, CANVAS_KF_CONTROL));
    OESetBit(appleIIIKeyFlags, APPLEIII_NOT_OPENAPPLE, !OEGetBit(flags, CANVAS_KF_LEFTGUI));
    OESetBit(appleIIIKeyFlags, APPLEIII_NOT_CLOSEAPPLE, !OEGetBit(flags, CANVAS_KF_RIGHTGUI));
}

void AppleIIIKeyboard::sendKey(CanvasUnicodeChar key)
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
        key = 0x89;
    else if (key == 0x1b)
        key = 0x9b;
    else if (key == 0x20)
        key = 0xa0;
    else if (key == 0x7f)
        key = 0x88;
    else if (key >= 0x80)
        return;
    
    if (pressedKeypadKeyCount)
        key |= 0x80;
    
    if (key >= 'a' && key <= 'z')
        key -= 0x20;
    
    keyLatch = key;
    setKeyStrobe(true);
}

void AppleIIIKeyboard::setReset(bool value)
{
    if (value)
        controlBus->postMessage(this, CONTROLBUS_ASSERT_NMI, NULL);
    else
        controlBus->postMessage(this, CONTROLBUS_CLEAR_NMI, NULL);
}

void AppleIIIKeyboard::setAltReset(bool value)
{
    if (value)
        controlBus->postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
    else
        controlBus->postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
}

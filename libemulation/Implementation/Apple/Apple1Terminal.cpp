
/**
 * libemulation
 * Apple-1 Terminal
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 terminal
 */

#include "Apple1Terminal.h"

#include "DeviceInterface.h"
#include "RS232Interface.h"
#include "MemoryInterface.h"

#define SCREEN_ORIGIN_X     104
#define SCREEN_ORIGIN_Y     25
#define SCREEN_WIDTH        768
#define SCREEN_HEIGHT       242

#define CHAR_WIDTH          14
#define CHAR_HEIGHT         8

#define BLOCK_WIDTH         40
#define BLOCK_HEIGHT        24

#define FONT_SIZE           0x80
#define FONT_SIZE_MASK      0x7f
#define FONT_WIDTH          16
#define FONT_HEIGHT         8

#define BLINK_ON            20
#define BLINK_OFF           10

Apple1Terminal::Apple1Terminal()
{
    device = NULL;
    controlBus = NULL;
    vram = NULL;
    monitorDevice = NULL;
    monitor = NULL;
    
    vramp = NULL;
    cursorX = 0;
    cursorY = 0;
    clearScreenOnCtrlL = false;
    splashScreen = false;
    splashScreenActive = false;
    
    canvasShouldUpdate = true;
    image.setFormat(OEIMAGE_LUMINANCE);
    image.setSize(OEMakeSize(SCREEN_WIDTH, SCREEN_HEIGHT));
    cursorActive = false;
    cursorCount = 0;
    
    powerState = CONTROLBUS_POWERSTATE_ON;
}

bool Apple1Terminal::setValue(string name, string value)
{
    if (name == "cursorX")
        cursorX = getOEInt(value);
    else if (name == "cursorY")
        cursorY = getOEInt(value);
    else if (name == "clearScreenOnCtrlL")
        clearScreenOnCtrlL = getOEInt(value);
    else if (name == "splashScreen")
        splashScreen = getOEInt(value);
    else if (name == "splashScreenActive")
        splashScreenActive = getOEInt(value);
    else
        return false;
    
    return true;
}

bool Apple1Terminal::getValue(string name, string& value)
{
    if (name == "cursorX")
        value = getString(cursorX);
    else if (name == "cursorY")
        value = getString(cursorY);
    else if (name == "splashScreenActive")
        value = getString(splashScreenActive);
    else
        return false;
    
    return true;
}

bool Apple1Terminal::setRef(string name, OEComponent *ref)
{
    if (name == "device")
    {
        if (device)
            device->removeObserver(this, DEVICE_DID_CHANGE);
        device = ref;
        if (device)
            device->addObserver(this, DEVICE_DID_CHANGE);
    }
    else if (name == "controlBus")
    {
        if (controlBus)
        {
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
            controlBus->removeObserver(this, CONTROLBUS_RESET_DID_ASSERT);
            controlBus->removeObserver(this, CONTROLBUS_TIMER_DID_FIRE);
        }
        controlBus = ref;
        if (controlBus)
        {
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
            controlBus->addObserver(this, CONTROLBUS_RESET_DID_ASSERT);
            controlBus->addObserver(this, CONTROLBUS_TIMER_DID_FIRE);
        }
    }
    else if (name == "vram")
        vram = ref;
    else if (name == "monitorDevice")
    {
        if (monitorDevice)
            monitorDevice->removeObserver(this, DEVICE_DID_CHANGE);
        monitorDevice = ref;
        if (monitorDevice)
            monitorDevice->addObserver(this, DEVICE_DID_CHANGE);
    }
    else if (name == "monitor")
    {
        if (monitor)
        {
            monitor->removeObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
            monitor->removeObserver(this, CANVAS_DID_COPY);
            monitor->removeObserver(this, CANVAS_DID_PASTE);
        }
        monitor = ref;
        if (monitor)
        {
            monitor->addObserver(this, CANVAS_UNICODECHAR_WAS_SENT);
            monitor->addObserver(this, CANVAS_DID_COPY);
            monitor->addObserver(this, CANVAS_DID_PASTE);
        }
    }
    else
        return false;
    
    return true;
}

bool Apple1Terminal::setData(string name, OEData *data)
{
    if (name == "font")
        loadFont(data);
    else
        return false;
    
    return true;
}

bool Apple1Terminal::init()
{
    if (!device)
    {
        logMessage("device not connected");
        
        return false;
    }
    
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!vram)
    {
        logMessage("vram not connected");
        
        return false;
    }
    
    OEData *vramData;
    
    vram->postMessage(this, RAM_GET_DATA, &vramData);
    
    if (vramData->size() < (BLOCK_WIDTH * BLOCK_HEIGHT))
    {
        logMessage("not enough vram");
        
        return false;
    }
    
    vramp = &vramData->front();
    
    if (!font.size())
    {
        logMessage("font not loaded");
        
        return false;
    }
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    scheduleNextTimer(0);
    
    canvasShouldUpdate = true;
    
    return true;
}

void Apple1Terminal::update()
{
    canvasShouldUpdate = true;
}

void Apple1Terminal::dispose()
{
    vramp = NULL;
}

bool Apple1Terminal::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case RS232_SEND_DATA:
        {
            OEData *theData = (OEData *)data;
            
            for (OEData::iterator i = theData->begin();
                 i != theData->end();
                 i++)
                putChar(*i);
            
            return true;
        }
            
        case RS232_ASSERT_RTS:
            isRTS = true;
            
            emptyPasteBuffer();
            
            return true;
            
        case RS232_CLEAR_RTS:
            isRTS = false;
            
            return true;
    }
    
    return false;
}

void Apple1Terminal::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == device)
    {
        if (*((DeviceEvent *)data) == DEVICE_WARMRESTART)
        {
            if (splashScreenActive)
            {
                splashScreenActive = false;
                
                controlBus->postMessage(this, CONTROLBUS_CLEAR_RESET, NULL);
                
                clearScreen();
            }
        }
    }
    else if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
                if (powerState == CONTROLBUS_POWERSTATE_OFF)
                {
                    if (splashScreen && !splashScreenActive)
                    {
                        splashScreenActive = true;
                        
                        controlBus->postMessage(this, CONTROLBUS_ASSERT_RESET, NULL);
                    }
                    
                    canvasShouldUpdate = true;
                }
                
                powerState = *((ControlBusPowerState *)data);
                
                if (powerState == CONTROLBUS_POWERSTATE_OFF)
                    clearScreen();
                
                break;
                
            case CONTROLBUS_RESET_DID_ASSERT:
                // Clear pastebuffer
                while (!pasteBuffer.empty())
                    pasteBuffer.pop();
                
                break;
                
            case CONTROLBUS_TIMER_DID_FIRE:
                vsync();
                
                scheduleNextTimer(*((OESLong *) data));
                
                postNotification(this, RS232_CTS_DID_ASSERT, NULL);
                postNotification(this, RS232_CTS_DID_CLEAR, NULL);
                
                break;
        }
    }
    else if (sender == monitorDevice)
        device->postNotification(sender, notification, data);
    else if (sender == monitor)
    {
        switch (notification)
        {
            case CANVAS_UNICODECHAR_WAS_SENT:
            {
                if (!pasteBuffer.empty())
                    break;
                
                CanvasUnicodeChar key = *((CanvasUnicodeChar *)data);
                
                if (((key == 0x0c) && clearScreenOnCtrlL) ||
                    (key == 0x7f))
                    clearScreen();
                else if (key == 0x08)
                    sendKey('_');
                else if (key <= 0x80)
                    sendKey(key);
                
                break;
            }
                
            case CANVAS_DID_COPY:
                copy((wstring *)data);
                
                break;
                
            case CANVAS_DID_PASTE:
                paste((wstring *)data);
                
                break;
        }
    }
}

void Apple1Terminal::scheduleNextTimer(OESLong cycles)
{
    cycles += 262 * 61;
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
}

void Apple1Terminal::loadFont(OEData *data)
{
    if (data->size() < FONT_HEIGHT)
        return;
    
    int cMask = (int) getNextPowerOf2(data->size() / FONT_HEIGHT) - 1;
    
    font.resize(FONT_SIZE * FONT_HEIGHT * FONT_WIDTH);
    
    for (int c = 0; c < FONT_SIZE; c++)
    {
        for (int y = 0; y < FONT_HEIGHT; y++)
        {
            for (int x = 0; x < FONT_WIDTH; x++)
            {
                bool b = (data->at((c & cMask) * FONT_HEIGHT + y) << (x >> 1)) & 0x40;
                
                font[(c * FONT_HEIGHT + y) * FONT_WIDTH + x] = b ? 0xff : 0x00;
            }
        }
    }
}

// Copy a 14-pixel segment
#define copySegment(x) \
*((OELong *)(p + x * SCREEN_WIDTH + 0)) = *((OELong *)(f + x * FONT_WIDTH + 0));\
*((OEInt *)(p + x * SCREEN_WIDTH + 8)) = *((OEInt *)(f + x * FONT_WIDTH + 8));\
*((OEShort *)(p + x * SCREEN_WIDTH + 12)) = *((OEShort *)(f + x * FONT_WIDTH + 12));

void Apple1Terminal::vsync()
{
    if (powerState == CONTROLBUS_POWERSTATE_OFF)
        return;
    
    if (splashScreenActive)
        cursorActive = false;
    else if (powerState == CONTROLBUS_POWERSTATE_ON)
    {
        if (cursorCount)
            cursorCount--;
        else
        {
            cursorActive = !cursorActive;
            cursorCount = cursorActive ? BLINK_ON : BLINK_OFF;
            
            canvasShouldUpdate = true;
        }
    }
    
    if (!canvasShouldUpdate)
        return;
    
    if (!vramp)
        return;
    
    OEChar *fp = (OEChar *)&font.front();
    OEChar *ip = (OEChar *)image.getPixels();
    
    // Place cursor
    OEChar cursorChar = vramp[cursorY * BLOCK_WIDTH + cursorX];
    if (cursorActive)
        vramp[cursorY * BLOCK_WIDTH + cursorX] = '@';
    
    for (int y = 0; y < BLOCK_HEIGHT; y++)
    {
        OEChar *p = (ip + y * SCREEN_WIDTH * CHAR_HEIGHT +
                     SCREEN_ORIGIN_Y * SCREEN_WIDTH +
                     SCREEN_ORIGIN_X);
        
        for (int x = 0; x < BLOCK_WIDTH; x++)
        {
            OEChar i = vramp[y * BLOCK_WIDTH + x] & FONT_SIZE_MASK;
            OEChar *f = fp + i * FONT_HEIGHT * FONT_WIDTH;
            
            copySegment(0);
            copySegment(1);
            copySegment(2);
            copySegment(3);
            copySegment(4);
            copySegment(5);
            copySegment(6);
            copySegment(7);
            
            p += CHAR_WIDTH;
        }
    }
    
    if (monitor)
        monitor->postMessage(this, CANVAS_POST_IMAGE, &image);
    
    // Remove cursor
    vramp[cursorY * BLOCK_WIDTH + cursorX] = cursorChar;
    
    canvasShouldUpdate = false;
}

void Apple1Terminal::clearScreen()
{
    if (!vramp)
        return;
    
    memset(vramp, ' ', BLOCK_HEIGHT * BLOCK_WIDTH);
    
    cursorX = 0;
    cursorY = 0;
    
    canvasShouldUpdate = true;
}

void Apple1Terminal::putChar(OEChar c)
{
    if (!vramp)
        return;
    
    if (c == 0x0d)
    {
        cursorX = 0;
        cursorY++;
        
        canvasShouldUpdate = true;
    }
    else if ((c >= 0x20) && (c <= 0x7f))
    {
        vramp[cursorY * BLOCK_WIDTH + cursorX] = c;
        
        cursorX++;
        if (cursorX >= BLOCK_WIDTH)
        {
            cursorX = 0;
            cursorY++;
        }
        
        canvasShouldUpdate = true;
    }
    
    if (cursorY >= BLOCK_HEIGHT)
    {
        cursorY = BLOCK_HEIGHT - 1;
        
        memmove(vramp, vramp + BLOCK_WIDTH, (BLOCK_HEIGHT - 1) * BLOCK_WIDTH);
        memset(vramp + (BLOCK_HEIGHT - 1) * BLOCK_WIDTH, ' ', BLOCK_WIDTH);
        
        canvasShouldUpdate = true;
    }
}

void Apple1Terminal::sendKey(CanvasUnicodeChar key)
{
    if (key >= 0x80)
        return;
    
    if (key == '\n')
        key = '\r';
    else if (key == '\r')
        return;
    
    OEData data;
    data.push_back(key);
    
    postNotification(this, RS232_DID_RECEIVE_DATA, &data);
}

void Apple1Terminal::copy(wstring *s)
{
    if (!vramp)
        return;
    
    for (int y = 0; y < BLOCK_HEIGHT; y++)
    {
        wstring line;
        
        for (int x = 0; x < BLOCK_WIDTH; x++)
            line += vramp[y * BLOCK_WIDTH + x] & 0x7f;
        
        line = rtrim(line);
        line += '\n';
        
        *s += line;
    }
}

void Apple1Terminal::paste(wstring *s)
{
    for (int i = 0; i < s->size(); i++)
    {
        if (s->at(i) <= 0x80)
            pasteBuffer.push(s->at(i));
    }
    
    emptyPasteBuffer();
}

void Apple1Terminal::emptyPasteBuffer()
{
    while (isRTS && !pasteBuffer.empty())
    {
        sendKey(pasteBuffer.front());
        
        pasteBuffer.pop();
    }
}

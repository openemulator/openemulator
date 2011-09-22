
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
#include "RAM.h"

#define SCREEN_ORIGIN_X     104
#define SCREEN_ORIGIN_Y     25
#define SCREEN_WIDTH        768
#define SCREEN_HEIGHT       242
#define TERM_WIDTH          40
#define TERM_HEIGHT         24
#define CHAR_WIDTH          14
#define CHAR_HEIGHT         8
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
    
    cursorActive = false;
    cursorCount = 0;
    cursorX = 0;
    cursorY = 0;
    
    vramp = NULL;
    image.setFormat(OEIMAGE_LUMINANCE);
    image.setSize(OEMakeSize(SCREEN_WIDTH, SCREEN_HEIGHT));
}

bool Apple1Terminal::setValue(string name, string value)
{
    if (name == "cursorX")
        cursorX = (OEUInt32) getUInt(value);
    else if (name == "cursorY")
        cursorY = (OEUInt32) getUInt(value);
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
    else
        return false;
    
    return true;
}

bool Apple1Terminal::setRef(string name, OEComponent *ref)
{
    if (name == "device")
        device = ref;
    else if (name == "vram")
        vram = ref;
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
    else if (name == "monitorDevice")
    {
        if (monitorDevice)
            monitorDevice->removeObserver(this, DEVICE_EVENT_DID_OCCUR);
        monitorDevice = ref;
        if (monitorDevice)
            monitorDevice->addObserver(this, DEVICE_EVENT_DID_OCCUR);
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
            
            updateCanvas();
            updateBezel();
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
    
    if (!vram)
    {
        logMessage("vram not connected");
        
        return false;
    }
    
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    OEData *vramData;
    vram->postMessage(this, RAM_GET_MEMORY, &vramData);
    if (vramData->size() < (TERM_WIDTH * TERM_HEIGHT))
    {
        logMessage("not enough vram");
        
        return false;
    }
    vramp = (OEUInt8 *)&vramData->front();
    
    if (!font.size())
    {
        logMessage("font not loaded");
        
        return false;
    }
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    updateCanvas();
    updateBezel();
    
    scheduleTimer();
    
    return true;
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
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
                powerState = *((ControlBusPowerState *)data);
                if (powerState == CONTROLBUS_POWERSTATE_OFF)
                {
                    clearScreen();
                    updateBezel();
                    
                    if (monitor)
                        monitor->postMessage(this, CANVAS_CLEAR, NULL);
                }
                
                break;
                
            case CONTROLBUS_RESET_DID_ASSERT:
                // Empty pastebuffer
                while (!pasteBuffer.empty())
                    pasteBuffer.pop();
                
                break;
                
            case CONTROLBUS_TIMER_DID_FIRE:
                updateCanvas();
                scheduleTimer();
                
                OEComponent::notify(this, RS232_CTS_DID_ASSERT, NULL);
                OEComponent::notify(this, RS232_CTS_DID_CLEAR, NULL);
                
                break;
        }
    }
    else if (sender == monitorDevice)
        device->notify(sender, notification, data);
    else if (sender == monitor)
    {
        switch (notification)
        {
            case CANVAS_UNICODECHAR_WAS_SENT:
            {
                if (!pasteBuffer.empty())
                    break;
                
                CanvasUnicodeChar key = *((CanvasUnicodeChar *)data);
                
                if (key == 0x7f)
                    clearScreen();
                else
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

void Apple1Terminal::scheduleTimer()
{
    OEUInt64 clocks = 262 * 61;
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &clocks);
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

// Copy a 14-pixel char scanline with 3 ints and one short
#define copyCharLine(x) \
*((OEUInt64 *)(p + x * SCREEN_WIDTH + 0)) = *((OEUInt64 *)(f + x * FONT_WIDTH + 0));\
*((OEUInt32 *)(p + x * SCREEN_WIDTH + 8)) = *((OEUInt32 *)(f + x * FONT_WIDTH + 8));\
*((OEUInt16 *)(p + x * SCREEN_WIDTH + 12)) = *((OEUInt16 *)(f + x * FONT_WIDTH + 12));

void Apple1Terminal::updateCanvas()
{
    if (!monitor)
        return;
    
    if (!vramp)
        return;
    
    // No updates when power is turned off
    if (powerState == CONTROLBUS_POWERSTATE_OFF)
        return;
    
    if (powerState == CONTROLBUS_POWERSTATE_ON)
    {
        if (cursorCount)
            cursorCount--;
        else
        {
            cursorActive = !cursorActive;
            cursorCount = cursorActive ? BLINK_ON : BLINK_OFF;
        }
    }
    // Hide cursor when paused
    else
        cursorActive = false;
    
    OEUInt8 *fp = (OEUInt8 *)&font.front();
    OEUInt8 *ip = (OEUInt8 *)image.getPixels();
    
    for (int y = 0; y < TERM_HEIGHT; y++)
        for (int x = 0; x < TERM_WIDTH; x++)
        {
            OEUInt8 c = vramp[y * TERM_WIDTH + x] & FONT_SIZE_MASK;
            
            if (cursorActive &&
                (x == cursorX) &&
                (y == cursorY))
                c = 0x40;
            
            OEUInt8 *f = fp + c * FONT_HEIGHT * FONT_WIDTH;
            OEUInt8 *p = (ip + y * SCREEN_WIDTH * CHAR_HEIGHT +
                       x * CHAR_WIDTH + SCREEN_ORIGIN_Y * SCREEN_WIDTH + SCREEN_ORIGIN_X);
            
            copyCharLine(0);
            copyCharLine(1);
            copyCharLine(2);
            copyCharLine(3);
            copyCharLine(4);
            copyCharLine(5);
            copyCharLine(6);
            copyCharLine(7);
        }
    
    monitor->postMessage(this, CANVAS_POST_IMAGE, &image);
}

void Apple1Terminal::updateBezel()
{
    if (!monitor)
        return;
    
    CanvasBezel bezel;
    switch (powerState)
    {
        case CONTROLBUS_POWERSTATE_OFF:
            bezel = CANVAS_BEZEL_POWER;
            break;
            
        case CONTROLBUS_POWERSTATE_PAUSE:
            bezel = CANVAS_BEZEL_PAUSE;
            break;
            
        default:
            bezel = CANVAS_BEZEL_NONE;
            break;
    }
    
    monitor->postMessage(this, CANVAS_SET_BEZEL, &bezel);
}

void Apple1Terminal::putChar(OEUInt8 c)
{
    if (c == 0x0d)
    {
        cursorX = 0;
        cursorY++;
    }
    else if ((c >= 0x20) && (c <= 0x7f))
    {
        vramp[cursorY * TERM_WIDTH + cursorX] = c;
        
        cursorX++;
        if (cursorX >= TERM_WIDTH)
        {
            cursorX = 0;
            cursorY++;
        }
    }
    
    if (cursorY >= TERM_HEIGHT)
    {
        cursorY = TERM_HEIGHT - 1;
        
        memmove(vramp, vramp + TERM_WIDTH, (TERM_HEIGHT - 1) * TERM_WIDTH);
        memset(vramp + (TERM_HEIGHT - 1) * TERM_WIDTH, 0x20, TERM_WIDTH);
    }
}

void Apple1Terminal::clearScreen()
{
    memset(vramp, 0x20, TERM_HEIGHT * TERM_WIDTH);
    
    cursorX = 0;
    cursorY = 0;
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
    
    OEComponent::notify(this, RS232_DID_RECEIVE_DATA, &data);
}

void Apple1Terminal::copy(wstring *s)
{
    for (int y = 0; y < TERM_HEIGHT; y++)
    {
        wstring line;
        
        for (int x = 0; x < TERM_WIDTH; x++)
            line += (vramp[y * TERM_WIDTH + x]) & 0x7f;
        
        line = rtrim(line);
        line += '\n';
        
        *s += line;
    }
}

void Apple1Terminal::paste(wstring *s)
{
    for (int i = 0; i < s->size(); i++)
        pasteBuffer.push(s->at(i));
    
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

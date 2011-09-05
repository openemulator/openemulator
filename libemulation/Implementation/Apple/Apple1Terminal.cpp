
/**
 * libemulation
 * Apple I Terminal
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I Terminal
 */

#include "Apple1Terminal.h"

#include "DeviceInterface.h"
#include "RAM.h"

#define SCREEN_ORIGIN_X 104
#define SCREEN_ORIGIN_Y 25
#define SCREEN_WIDTH 768
#define SCREEN_HEIGHT 242
#define TERM_WIDTH 40
#define TERM_HEIGHT 24
#define CHAR_WIDTH 14
#define CHAR_HEIGHT 8
#define FONT_SIZE 0x40
#define FONT_WIDTH 16
#define FONT_HEIGHT 8

Apple1Terminal::Apple1Terminal()
{
    device = NULL;
    controlBus = NULL;
    vram = NULL;
    monitorDevice = NULL;
    monitor = NULL;
    
    speedLimit = true;
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
    if (name == "terminalSpeed")
        speedLimit = (value == "Standard");
    else if (name == "cursorX")
        cursorX = getUInt32(value);
    else if (name == "cursorY")
        cursorY = getUInt32(value);
    else
        return false;
    
    return true;
}

bool Apple1Terminal::getValue(string name, string& value)
{
    if (name == "terminalSpeed")
        value = speedLimit ? "Standard" : "Enhanced";
    else if (name == "cursorX")
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
            controlBus->removeObserver(this, CONTROLBUS_TIMER_DID_FIRE);
        }
        controlBus = ref;
        if (controlBus)
        {
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
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
    vramp = &vramData->front();
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    updateCanvas();
    updateBezel();
    
    scheduleTimer();
    
    return true;
}

void Apple1Terminal::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_TIMER_DID_FIRE:
                updateCanvas();
                scheduleTimer();
                break;
                
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
            {
                powerState = *((ControlBusPowerState *)data);
                if (monitor)
                {
                    updateBezel();
                    
                    if (powerState == CONTROLBUS_POWERSTATE_OFF)
                    {
                        monitor->postMessage(this, CANVAS_CLEAR, NULL);
                        
                        cursorX = 0;
                        cursorY = 0;
                    }
                }
                
                break;
            }
        }
    }
    else if (sender == monitorDevice)
        device->notify(sender, notification, data);
    else if (sender == monitor)
    {
        switch (notification)
        {
            case CANVAS_UNICODECHAR_WAS_SENT:
                sendUnicodeChar(*((CanvasUnicodeChar *)data));
                break;
                
            case CANVAS_DID_COPY:
                copy((string *)data);
                break;
                
            case CANVAS_DID_PASTE:
                paste((string *)data);
                break;
        }
    }
}

void Apple1Terminal::write(int address, int value)
{
}

void Apple1Terminal::scheduleTimer()
{
    OEUInt64 clocks = 262 * 57;
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &clocks);
}

void Apple1Terminal::loadFont(OEData *data)
{
    font.resize(FONT_SIZE * FONT_HEIGHT * FONT_WIDTH);
    
    for (int i = 0; i < FONT_SIZE; i++)
    {
        for (int y = 0; y < FONT_HEIGHT; y++)
        {
            for (int x = 0; x < FONT_WIDTH; x++)
            {
                bool b = (data->at(i * FONT_HEIGHT + y) >> (x >> 1)) & 0x1;
                
                font[(i * FONT_HEIGHT + y) * FONT_WIDTH + x] = b ? 0xff : 0x00;
            }
        }
    }
}

// Copy a 14-pixel char scanline with 3 ints and one short
#define copyCharLine(x) \
*((int *)(p + x * SCREEN_WIDTH + 0)) = *((int *)(f + x * FONT_WIDTH + 0));\
*((int *)(p + x * SCREEN_WIDTH + 4)) = *((int *)(f + x * FONT_WIDTH + 4));\
*((int *)(p + x * SCREEN_WIDTH + 8)) = *((int *)(f + x * FONT_WIDTH + 8));\
*((short *)(p + x * SCREEN_WIDTH + 12)) = *((short *)(f + x * FONT_WIDTH + 12));

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
            cursorCount = cursorActive ? 10 : 20;
        }
    }
    // Hide cursor when paused
    else
        cursorActive = false;
    
    char *fp = (char *)&font.front();
    char *ip = (char *)image.getPixels();
    
    for (int y = 0; y < TERM_HEIGHT; y++)
        for (int x = 0; x < TERM_WIDTH; x++)
        {
            int c = vramp[y * TERM_WIDTH + x] & (FONT_SIZE - 1);
            if (cursorActive && (x == cursorX) && (y == cursorY))
                c = 0;
            
            char *f = fp + c * FONT_HEIGHT * FONT_WIDTH;
            char *p = (ip + y * SCREEN_WIDTH * CHAR_HEIGHT +
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

void Apple1Terminal::sendUnicodeChar(CanvasUnicodeChar unicodeChar)
{
    if (powerState != CONTROLBUS_POWERSTATE_ON)
        return;
    
    if (unicodeChar == 0x0a)
    {
        cursorX = 0;
        cursorY++;
    }
    else if (unicodeChar == 0x0c)
    {
        memset(vramp, 0x20, TERM_HEIGHT * TERM_WIDTH);
        
        cursorX = 0;
        cursorY = 0;
    }
    else if ((unicodeChar >= 0x20) && (unicodeChar <= 0x7f))
    {
        vramp[cursorY * TERM_WIDTH + cursorX] = unicodeChar;
        
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

void Apple1Terminal::copy(string *s)
{
    for (int y = 0; y < TERM_HEIGHT; y++)
    {
        string line;
        
        for (int x = 0; x < TERM_WIDTH; x++)
            line += (vramp[y * TERM_WIDTH + x]) & 0x7f;
        
        *s += rtrim(line) + '\n';
    }
}

void Apple1Terminal::paste(string *s)
{
    wstring ws = getWString(*s);
    
    for (int i = 0; i < ws.size(); i++)
        sendUnicodeChar(ws[i]);
}

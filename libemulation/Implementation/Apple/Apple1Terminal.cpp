
/**
 * libemulation
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "Apple1Terminal.h"

#include "RAM.h"
#include "ControlBus.h"
#include "CanvasInterface.h"

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
    vram = NULL;
    controlBus = NULL;
    monitor = NULL;
    
    speedLimit = true;
    cursorActive = true;
    cursorX = 0;
    cursorY = 0;
    
    vramp = NULL;
    image.setFormat(OEIMAGE_LUMINANCE);
    image.setOptions(OEIMAGE_COLORCARRIER);
    image.setSize(OEMakeSize(SCREEN_WIDTH, SCREEN_HEIGHT));
}

bool Apple1Terminal::setValue(string name, string value)
{
    
    if (name == "terminalSpeed")
        speedLimit = (value == "Standard");
    else
        return false;
    
    return true;
}

bool Apple1Terminal::getValue(string name, string& value)
{
    if (name == "terminalSpeed")
        value = speedLimit ? "Standard" : "Enhanced";
    else
        return false;
    
    return true;
}

bool Apple1Terminal::setRef(string name, OEComponent *ref)
{
    if (name == "vram")
        vram = ref;
    else if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_TIMER_DID_FIRE);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_TIMER_DID_FIRE);
    }
	else if (name == "monitor")
    {
        if (monitor)
        {
            monitor->removeObserver(this, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
            monitor->removeObserver(this, CANVAS_DID_COPY);
            monitor->removeObserver(this, CANVAS_DID_PASTE);
        }
		monitor = ref;
        if (monitor)
        {
            monitor->addObserver(this, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
            monitor->addObserver(this, CANVAS_DID_COPY);
            monitor->addObserver(this, CANVAS_DID_PASTE);
            updateCanvas();
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
    
    scheduleTimer();
    updateCanvas();
    
    return true;
}

void Apple1Terminal::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
        scheduleTimer();
    else if (sender == monitor)
    {
        switch (notification)
        {
            case CANVAS_UNICODEKEYBOARD_DID_CHANGE:
                sendKey(*((int *)data));
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
    int clock = 525 * 65;
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &clock);
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

// We just copy 3 int's, and leave the last 2 pixels
#define copyCharLine(x) \
*((int *)(p + x * SCREEN_WIDTH + 0)) = *((int *)(f + x * FONT_WIDTH + 0));\
*((int *)(p + x * SCREEN_WIDTH + 4)) = *((int *)(f + x * FONT_WIDTH + 4));\
*((int *)(p + x * SCREEN_WIDTH + 8)) = *((int *)(f + x * FONT_WIDTH + 8));

void Apple1Terminal::updateCanvas()
{
    if (!monitor)
        return;
    
    if (!vramp)
        return;
    
    for (int y = 0; y < TERM_HEIGHT; y++)
        for (int x = 0; x < TERM_WIDTH; x++)
        {
            int c = vramp[y * TERM_WIDTH + x] & (FONT_SIZE - 1);
            if (cursorActive && (x == cursorX) && (y == cursorY))
                c = 0;
            
            char *f = (char *)&font.front() + c * FONT_HEIGHT * FONT_WIDTH;
            char *p = ((char *)image.getPixels() + y * SCREEN_WIDTH * CHAR_HEIGHT +
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

void Apple1Terminal::sendKey(int unicode)
{
    if (unicode >= 0x80)
        return;
    
    if (unicode == 0x0a)
    {
        cursorX = 0;
        cursorY++;
    }
    else if (unicode == 0x0c)
    {
        memset(vramp, 0x20, TERM_HEIGHT * TERM_WIDTH);
        
        cursorX = 0;
        cursorY = 0;
    }
    else if (unicode <= 0x7f)
    {
        vramp[cursorY * TERM_WIDTH + cursorX] = unicode;
        
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
    
    updateCanvas();
}

void Apple1Terminal::copy(string *s)
{
    for (int y = 0; y < TERM_HEIGHT; y++)
    {
        string line;
        
        for (int x = 0; x < TERM_WIDTH; x++)
            line += vramp[y * TERM_WIDTH + x];
        
        *s += rtrim(line) + '\n';
    }
}

void Apple1Terminal::paste(string *s)
{
    for (int i = 0; i < s->size(); i++)
        sendKey((*s)[i]);
}

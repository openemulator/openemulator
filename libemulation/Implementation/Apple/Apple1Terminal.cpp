
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

#define SCREEN_OFFSET 8
#define SCREEN_WIDTH 576
#define SCREEN_HEIGHT 192
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
    cursorX = 0;
    cursorY = 0;
    
    image.setFormat(OEIMAGE_LUMINANCE);
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
            monitor->removeObserver(this, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
		monitor = ref;
        if (monitor)
        {
            monitor->addObserver(this, CANVAS_UNICODEKEYBOARD_DID_CHANGE);
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
    
    scheduleTimer();
    updateCanvas();
    
    return true;
}

void Apple1Terminal::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
        scheduleTimer();
    else if (sender == monitor)
        sendKey(*((int *)data));
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

#define copyCharLine(x) \
    *((int *)(p + x * SCREEN_WIDTH + 0)) = *((int *)(f + x * FONT_WIDTH + 0));\
    *((int *)(p + x * SCREEN_WIDTH + 4)) = *((int *)(f + x * FONT_WIDTH + 4));\
    *((int *)(p + x * SCREEN_WIDTH + 8)) = *((int *)(f + x * FONT_WIDTH + 8));\

void Apple1Terminal::updateCanvas()
{
    if (!monitor)
        return;
    
    OEData *vramData;
    vram->postMessage(this, RAM_GET_MEMORY, &vramData);
    if (vramData->size() < (TERM_WIDTH * TERM_HEIGHT))
        return;
    
    for (int y = 0; y < TERM_HEIGHT; y++)
        for (int x = 0; x < TERM_WIDTH; x++)
        {
            int c = (*vramData)[y * TERM_WIDTH + x] & (FONT_SIZE - 1);
//            c = (x + y) & 0x3f;
            char *f = (char *)&font.front() + c * FONT_HEIGHT * FONT_WIDTH;
            char *p = ((char *)image.getPixels() + y * SCREEN_WIDTH * CHAR_HEIGHT +
                       x * CHAR_WIDTH + SCREEN_OFFSET);
            
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
    OEData *vramData;
    vram->postMessage(this, RAM_GET_MEMORY, &vramData);
    
    (*vramData)[cursorY * TERM_WIDTH + cursorX] = unicode;
    cursorX++;
    if (cursorX >= TERM_WIDTH)
    {
        cursorX = 0;
        cursorY++;
        
        if (cursorY >= TERM_HEIGHT)
            cursorY = 0;
    }
    
    updateCanvas();
}

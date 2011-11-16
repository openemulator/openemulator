
/**
 * libemulator
 * Apple II Video
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates Apple II video
 */

#include "AppleIIVideo.h"

#include "DeviceInterface.h"

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

AppleIIVideo::AppleIIVideo()
{
    device = NULL;
    controlBus = NULL;
    floatingBus = NULL;
    mmu = NULL;
    monitorDevice = NULL;
    monitor = NULL;
    
    rev0 = false;
    palTiming = false;
    characterSet = "Standard";
}

bool AppleIIVideo::setValue(string name, string value)
{
	if (name == "rev")
        rev0 = (value == "Revision 0");
	else if (name == "tvSystem")
		palTiming = (value == "PAL");
	else if (name == "characterSet")
		characterSet = value;
	else if (name == "text")
        updateMode(APPLEIIVIDEO_TEXT, getInt(value));
	else if (name == "mixed")
        updateMode(APPLEIIVIDEO_MIXED, getInt(value));
	else if (name == "page2")
        updateMode(APPLEIIVIDEO_PAGE2, getInt(value));
	else if (name == "hires")
        updateMode(APPLEIIVIDEO_HIRES, getInt(value));
	else
		return false;
	
	return true;
}

bool AppleIIVideo::getValue(string name, string& value)
{
    if (name == "rev")
        value = rev0 ? "Revision 0" : "Revision 1";
	else if (name == "tvSystem")
		value = palTiming ? "PAL" : "NTSC";
	else if (name == "characterSet")
		value = characterSet;
	else if (name == "text")
		value = getString(OEGetBit(mode, APPLEIIVIDEO_TEXT));
	else if (name == "mixed")
		value = getString(OEGetBit(mode, APPLEIIVIDEO_MIXED));
	else if (name == "page2")
		value = getString(OEGetBit(mode, APPLEIIVIDEO_PAGE2));
	else if (name == "hires")
		value = getString(OEGetBit(mode, APPLEIIVIDEO_HIRES));
	else
		return false;
	
	return true;
}

bool AppleIIVideo::setRef(string name, OEComponent *ref)
{
    if (name == "device")
        device = ref;
    else if (name == "controlBus")
    {
        if (controlBus)
            controlBus->removeObserver(this, CONTROLBUS_TIMER_DID_FIRE);
        controlBus = ref;
        if (controlBus)
            controlBus->addObserver(this, CONTROLBUS_TIMER_DID_FIRE);
    }
	else if (name == "floatingBus")
		floatingBus = ref;
	else if (name == "mmu")
    {
        if (mmu)
            mmu->removeObserver(this, APPLEIIMMU_VIDEOMEMORY_DID_CHANGE);
		mmu = ref;
        if (mmu)
            mmu->addObserver(this, APPLEIIMMU_VIDEOMEMORY_DID_CHANGE);
    }
    else if (name == "monitorDevice")
    {
        if (monitorDevice)
            monitorDevice->removeObserver(this, DEVICE_EVENT_DID_OCCUR);
        monitorDevice = ref;
        if (monitorDevice)
        {
            monitorDevice->addObserver(this, DEVICE_EVENT_DID_OCCUR);
            
            canvasShouldUpdate = true;
        }
    }
	else if (name == "monitor")
		monitor = ref;
	else
		return false;
	
	return true;
}

bool AppleIIVideo::setData(string name, OEData *data)
{
    return true;
}

bool AppleIIVideo::init()
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
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    if (!mmu)
    {
        logMessage("mmu not connected");
        
        return false;
    }
    
/*    OEData *data;
    vram->postMessage(this, RAM_GET_DATA, &data);
    if (vramData->size() < (TERM_WIDTH * TERM_HEIGHT))
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
    */
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    scheduleTimer();
    
    mmu->postMessage(this, APPLEIIMMU_GET_VIDEOMEMORY, &vram);
    
    return true;
}

void AppleIIVideo::dispose()
{
    
}

bool AppleIIVideo::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case APPLEIIVIDEO_GET_MODE:
            *((OEUInt32 *)data) = mode;
            
            return true;
    }
    
    return false;
}

void AppleIIVideo::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
                powerState = *((ControlBusPowerState *)data);
                
                break;
                
            case CONTROLBUS_TIMER_DID_FIRE:
                vsync();
                
                scheduleTimer();
                
                break;
        }
    }
    else if (sender == mmu)
        vram = *((AppleIIMMUVideoMemory *)data);
    else if (sender == monitorDevice)
        device->postNotification(sender, notification, data);
    else if (sender == monitor)
    {
        switch (notification)
        {
            case CANVAS_DID_COPY:
                copy((wstring *)data);
                
                break;
        }
    }
}

OEUInt8 AppleIIVideo::read(OEAddress address)
{
    write(address, 0);
    
	return floatingBus->read(address);
}

void AppleIIVideo::write(OEAddress address, OEUInt8 value)
{
    switch (address & 0x7f)
    {
        case 0x50: case 0x51:
            OESetBit(mode, APPLEIIVIDEO_TEXT, address & 0x1);
            
            break;
            
        case 0x52: case 0x53:
            OESetBit(mode, APPLEIIVIDEO_MIXED, address & 0x1);
            
            break;
            
        case 0x54: case 0x55:
            OESetBit(mode, APPLEIIVIDEO_PAGE2, address & 0x1);
            
            break;
            
        case 0x56: case 0x57:
            OESetBit(mode, APPLEIIVIDEO_HIRES, address & 0x1);
            
            break;
    }
}

void AppleIIVideo::updateMode(OEUInt32 mask, bool value)
{
    OEUInt32 oldMode = mode;
    
    OESetBit(mode, mask, value);
    
    if (mode != oldMode)
        postNotification(this, APPLEIIVIDEO_MODE_DID_CHANGE, &mode);
}

void AppleIIVideo::scheduleTimer()
{
    OEUInt64 clocks = 262 * 65;
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &clocks);
}

OEData AppleIIVideo::loadFont(OEData *data)
{
    OEData font;
    
    font.resize(FONT_SIZE * FONT_HEIGHT * FONT_WIDTH);
    
    if (data->size() >= FONT_HEIGHT)
    {
        int cMask = (int) getNextPowerOf2(data->size() / FONT_HEIGHT) - 1;
        
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
    
    return font;
}

// Copy a 14-pixel char scanline with 3 ints and one short
#define copyCharLine(x) \
*((OEUInt64 *)(p + x * SCREEN_WIDTH + 0)) = *((OEUInt64 *)(f + x * FONT_WIDTH + 0));\
*((OEUInt32 *)(p + x * SCREEN_WIDTH + 8)) = *((OEUInt32 *)(f + x * FONT_WIDTH + 8));\
*((OEUInt16 *)(p + x * SCREEN_WIDTH + 12)) = *((OEUInt16 *)(f + x * FONT_WIDTH + 12));

void AppleIIVideo::vsync()
{
/*    if (!monitor ||
        !textp ||
        (powerState == CONTROLBUS_POWERSTATE_OFF))
        return;
    
    if (powerState == CONTROLBUS_POWERSTATE_ON)
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
    
    OEUInt8 *fp = (OEUInt8 *)&font.front();
    OEUInt8 *ip = (OEUInt8 *)image.getPixels();
    
    // Generate cursor
    OEUInt8 cursorChar = vramp[cursorY * TERM_WIDTH + cursorX];
//    if (cursorActive)
//        textp[cursorY * TERM_WIDTH + cursorX] = '@';
    
    for (int y = 0; y < TERM_HEIGHT; y++)
        for (int x = 0; x < TERM_WIDTH; x++)
        {
            OEUInt8 c = vramp[y * TERM_WIDTH + x] & FONT_SIZE_MASK;
            OEUInt8 *f = fp + c * FONT_HEIGHT * FONT_WIDTH;
            OEUInt8 *p = (ip + y * SCREEN_WIDTH * CHAR_HEIGHT +
                          x * CHAR_WIDTH +
                          SCREEN_ORIGIN_Y * SCREEN_WIDTH +
                          SCREEN_ORIGIN_X);
            
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
    
    // Restore cursor char
//    textp[cursorY * TERM_WIDTH + cursorX] = cursorChar;
    
    canvasShouldUpdate = false;*/
}

void AppleIIVideo::copy(wstring *s)
{
/*    if (!vramp)
        return;
    
    for (int y = 0; y < TERM_HEIGHT; y++)
    {
        wstring line;
        
        for (int x = 0; x < TERM_WIDTH; x++)
            line += vramp[y * TERM_WIDTH + x] & 0x7f;
        
        line = rtrim(line);
        line += '\n';
        
        *s += line;
    }*/
}

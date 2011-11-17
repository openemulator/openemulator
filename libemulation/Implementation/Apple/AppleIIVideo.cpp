
/**
 * libemulator
 * Apple II Video
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates Apple II video
 */

#include <math.h>

#include "AppleIIVideo.h"

#include "DeviceInterface.h"
#include "MemoryInterface.h"
#include "CanvasInterface.h"
#include "AppleIIInterface.h"

#define SCREEN_ORIGIN_X     104
#define SCREEN_ORIGIN_Y     25
#define SCREEN_WIDTH        768
#define SCREEN_HEIGHT       242
#define TERM_WIDTH          40
#define TERM_HEIGHT         24
#define CHAR_WIDTH          14
#define CHAR_HEIGHT         8
#define FONT_SIZE           0x100
#define FONT_SIZE_MASK      0xff
#define FONT_WIDTH          16
#define FONT_HEIGHT         8
#define FLASH_HALFCYCLE     14

OEAddress appleIIVideoTextOffset[] = {
    0x000,
    0x080,
    0x100,
    0x180,
    0x200,
    0x280,
    0x300,
    0x380,
    0x028,
    0x0a8,
    0x128,
    0x1a8,
    0x228,
    0x2a8,
    0x328,
    0x3a8,
    0x050,
    0x0d0,
    0x150,
    0x1d0,
    0x250,
    0x2d0,
    0x350,
    0x3d0,
};

AppleIIVideo::AppleIIVideo()
{
    device = NULL;
    controlBus = NULL;
    floatingBus = NULL;
    monitorDevice = NULL;
    monitor = NULL;
    
    rev0 = false;
    palTiming = false;
    characterSet = "Standard";
    
    for (int i = 0; i < 2; i++)
        text[i] = NULL;
    for (int i = 0; i < 2; i++)
        hires[i] = NULL;
    canvasShouldUpdate = true;
    
    image.setFormat(OEIMAGE_LUMINANCE);
    image.setSize(OEMakeSize(SCREEN_WIDTH, SCREEN_HEIGHT));
    vector<float> colorBurst;
    colorBurst.push_back(M_PI * -33.0 / 180.0);
    image.setColorBurst(colorBurst);
    
    flash = false;
    flashCount = 0;
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
        OESetBit(mode, APPLEIIVIDEO_TEXT, getInt(value));
	else if (name == "mixed")
        OESetBit(mode, APPLEIIVIDEO_MIXED, getInt(value));
	else if (name == "page2")
        OESetBit(mode, APPLEIIVIDEO_PAGE2, getInt(value));
	else if (name == "hires")
        OESetBit(mode, APPLEIIVIDEO_HIRES, getInt(value));
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
    else if ((name == "ram1") || (name == "ram"))
        ram1 = ref;
    else if (name == "ram2")
        ram2 = ref;
    else if (name == "ram3")
        ram3 = ref;
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
    if (name.substr(0, 4) == "font")
        loadFont(name.substr(4), data);
    else
        return false;
    
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
    
    OEAddress startAddress = 0;
    
    getVRAM(ram1, startAddress);
    getVRAM(ram2, startAddress);
    getVRAM(ram3, startAddress);
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    scheduleTimer();
    
    return true;
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
            updateMode(APPLEIIVIDEO_TEXT, address & 0x1);
            
            break;
            
        case 0x52: case 0x53:
            updateMode(APPLEIIVIDEO_MIXED, address & 0x1);
            
            break;
            
        case 0x54: case 0x55:
            updateMode(APPLEIIVIDEO_PAGE2, address & 0x1);
            
            break;
            
        case 0x56: case 0x57:
            updateMode(APPLEIIVIDEO_HIRES, address & 0x1);
            
            break;
    }
}

void AppleIIVideo::getVRAM(OEComponent *ram, OEAddress& startAddress)
{
    if (!ram)
        return;
    
    OEData *data;
    ram->postMessage(this, RAM_GET_DATA, &data);
    
    OEAddress endAddress = startAddress + data->size() - 1;
    
    if ((startAddress <= 0x400) && (endAddress >= 0x7ff))
        text[0] = &data->front() + 0x400 - startAddress;
    if ((startAddress <= 0x800) && (endAddress >= 0xbff))
        text[1] = &data->front() + 0x800 - startAddress;
    if ((startAddress <= 0x2000) && (endAddress >= 0x3fff))
        hires[0] = &data->front() + 0x2000 - startAddress;
    if ((startAddress <= 0x4000) && (endAddress >= 0x5fff))
        hires[1] = &data->front() + 0x4000 - startAddress;
    
    startAddress += data->size();
}

void AppleIIVideo::scheduleTimer()
{
    OEUInt64 clocks = 262 * 65;
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &clocks);
}

void AppleIIVideo::loadFont(string name, OEData *data)
{
    if (data->size() < FONT_HEIGHT)
        return;
    
    OEData theFont;
    
    theFont.resize(2 * FONT_SIZE * FONT_HEIGHT * FONT_WIDTH);
    
    OEUInt8 *fp = &theFont[0];
    int cMask = (int) getNextPowerOf2(data->size() / FONT_HEIGHT) - 1;
    
    for (int c = 0; c < 2 * FONT_SIZE; c++)
    {
        for (int y = 0; y < FONT_HEIGHT; y++)
        {
            OEUInt8 byte = data->at((c & cMask) * FONT_HEIGHT + y);
            
            bool inv = !((c & 0x80) || ((byte & 0x80) && (c & 0x100)));
            
            for (int x = 0; x < FONT_WIDTH; x++)
            {
                bool b = (byte << (x >> 1)) & 0x40;
                
                fp[(c * FONT_HEIGHT + y) * FONT_WIDTH + x] = (b ^ inv) ? 0xff : 0x00;
            }
        }
    }
    
    font[name] = theFont;
}

void AppleIIVideo::updateMode(OEUInt32 mask, bool value)
{
    OEUInt32 oldMode = mode;
    
    OESetBit(mode, mask, value);
    
    if (mode != oldMode)
        postNotification(this, APPLEIIVIDEO_MODE_DID_CHANGE, &mode);
    
    if (rev0 || ((mode & ~APPLEIIVIDEO_PAGE2) == APPLEIIVIDEO_TEXT))
        image.setSubcarrier(3579545);
    else
        image.setSubcarrier(0);
}

// Copy a 14-pixel char scanline with 3 ints and one short
#define copyCharLine(x) \
*((OEUInt64 *)(p + x * SCREEN_WIDTH + 0)) = *((OEUInt64 *)(f + x * FONT_WIDTH + 0));\
*((OEUInt32 *)(p + x * SCREEN_WIDTH + 8)) = *((OEUInt32 *)(f + x * FONT_WIDTH + 8));\
*((OEUInt16 *)(p + x * SCREEN_WIDTH + 12)) = *((OEUInt16 *)(f + x * FONT_WIDTH + 12));

void AppleIIVideo::vsync()
{
    if (!monitor || (powerState == CONTROLBUS_POWERSTATE_OFF))
        return;
    
    if (powerState == CONTROLBUS_POWERSTATE_ON)
    {
        if (flashCount)
            flashCount--;
        else
        {
            flash = !flash;
            flashCount = FLASH_HALFCYCLE;
            
            canvasShouldUpdate = true;
        }
    }
    
    if (!canvasShouldUpdate)
        return;
    
    OEUInt8 *vp = text[OEGetBit(mode, APPLEIIVIDEO_PAGE2) ? 1 : 0];
    OEUInt8 *fp = (OEUInt8 *)&font[characterSet].front();
    OEUInt8 *ip = (OEUInt8 *)image.getPixels();
    
    if (!vp || !fp || !ip)
    {
        monitor->postMessage(this, CANVAS_CLEAR, NULL);
        
        return;
    }
    
    if (flash)
        fp += FONT_SIZE * FONT_WIDTH * FONT_HEIGHT;
    
    for (int y = 0; y < TERM_HEIGHT; y++)
        for (int x = 0; x < TERM_WIDTH; x++)
        {
            OEUInt8 c = vp[appleIIVideoTextOffset[y] + x];
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
    
    canvasShouldUpdate = false;
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

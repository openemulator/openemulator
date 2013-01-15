
/**
 * libemulation
 * Apple III Video
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III Video
 */

#include <math.h>

#include "AppleIIIVideo.h"

#include "DeviceInterface.h"
#include "MemoryInterface.h"
#include "AppleIIIInterface.h"

#define FRAMEBUFFER_WIDTH   560
#define FRAMEBUFFER_HEIGHT  192

#define HORIZ_START         16
#define HORIZ_BLANK         (9 + HORIZ_START)
#define HORIZ_DISPLAY       40
#define HORIZ_TOTAL         (HORIZ_BLANK + HORIZ_DISPLAY)

#define MODE_TEXT           (1 << 0)
#define MODE_MIXED          (1 << 1)
#define MODE_PAGE2          (1 << 2)
#define MODE_HIRES          (1 << 3)

#define CHARACTERSET_NUM    0x80
#define CHARACTERSET_HEIGHT 8
#define CHARACTERSET_SIZE   (CHARACTERSET_NUM * CHARACTERSET_HEIGHT)

#define FONT_CHARNUM        0x100
#define FONT_CHARWIDTH      16
#define FONT_CHARHEIGHT     8
#define FONT_CHARSIZE       (FONT_CHARWIDTH * FONT_CHARHEIGHT)
#define FONT_SIZE           (FONT_CHARNUM * FONT_CHARSIZE)

#define CELL_WIDTH          14
#define CELL_HEIGHT         8

#define VERT_NTSC_START     38
#define VERT_DISPLAY        192

#define BLOCK_WIDTH         HORIZ_DISPLAY
#define BLOCK_HEIGHT        (VERT_DISPLAY / CELL_HEIGHT)

enum
{
    TIMER_VSYNC,
    TIMER_DISPLAYMIXED,
    TIMER_DISPLAYEND,
};

AppleIIIVideo::AppleIIIVideo()
{
    controlBus = NULL;
    systemControl = NULL;
    vram = NULL;
    monochromeMonitor = NULL;
    compositeMonitor = NULL;
    rgbMonitor = NULL;
    
    flashFrameNum = 14;
    mode = 0;
    
    initOffsets();
    
    videoEnabled = false;
    colorKiller = true;
    
    buildLoresFont();
    buildHiresFont();
    
    framebuffer.resize(FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT);
    framebufferModified = false;
    
    frameStart = 0;
    frameCycleNum = 0;
    
    currentTimer = TIMER_VSYNC;
    lastCycles = 0;
    pendingCycles = 0;
    
    flash = false;
    flashCount = 0;
    
    powerState = CONTROLBUS_POWERSTATE_ON;
    monitorConnected = false;
    videoInhibitCount = 0;
    
    characterSet.resize(CHARACTERSET_SIZE);
    
    loadTextFont("", &characterSet);
}

bool AppleIIIVideo::setValue(string name, string value)
{
	if (name == "flashFrameNum")
		flashFrameNum = getOEInt(value);
	else if (name == "text")
        OESetBit(mode, MODE_TEXT, getOEInt(value));
	else if (name == "mixed")
        OESetBit(mode, MODE_MIXED, getOEInt(value));
	else if (name == "page2")
        OESetBit(mode, MODE_PAGE2, getOEInt(value));
	else if (name == "hires")
        OESetBit(mode, MODE_HIRES, getOEInt(value));
    else
        return false;
	
	return true;
}

bool AppleIIIVideo::getValue(string name, string& value)
{
	if (name == "text")
		value = getString(OEGetBit(mode, MODE_TEXT));
	else if (name == "mixed")
		value = getString(OEGetBit(mode, MODE_MIXED));
	else if (name == "page2")
		value = getString(OEGetBit(mode, MODE_PAGE2));
	else if (name == "hires")
		value = getString(OEGetBit(mode, MODE_HIRES));
	else
		return false;
	
	return true;
}

bool AppleIIIVideo::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
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
    else if (name == "systemControl")
    {
        if (systemControl)
            systemControl->removeObserver(this, APPLEIII_APPLEIIMODE_DID_CHANGE);
        systemControl = ref;
        if (systemControl)
            systemControl->addObserver(this, APPLEIII_APPLEIIMODE_DID_CHANGE);
    }
    else if (name == "vram")
        vram = ref;
	else if (name == "monochromeMonitor")
    {
        if (monochromeMonitor)
        {
            monochromeMonitor->removeObserver(this, CANVAS_MOUSE_DID_CHANGE);
            monochromeMonitor->removeObserver(this, CANVAS_DID_COPY);
        }
        monochromeMonitor = ref;
        if (monochromeMonitor)
        {
            monochromeMonitor->addObserver(this, CANVAS_MOUSE_DID_CHANGE);
            monochromeMonitor->addObserver(this, CANVAS_DID_COPY);
            
            CanvasCaptureMode captureMode = CANVAS_CAPTURE_ON_MOUSE_CLICK;
            monochromeMonitor->postMessage(this, CANVAS_SET_CAPTUREMODE, &captureMode);
        }
    }
	else if (name == "compositeMonitor")
    {
        if (compositeMonitor)
        {
            compositeMonitor->removeObserver(this, CANVAS_MOUSE_DID_CHANGE);
            compositeMonitor->removeObserver(this, CANVAS_DID_COPY);
        }
        compositeMonitor = ref;
        if (compositeMonitor)
        {
            compositeMonitor->addObserver(this, CANVAS_MOUSE_DID_CHANGE);
            compositeMonitor->addObserver(this, CANVAS_DID_COPY);
            
            CanvasCaptureMode captureMode = CANVAS_CAPTURE_ON_MOUSE_CLICK;
            compositeMonitor->postMessage(this, CANVAS_SET_CAPTUREMODE, &captureMode);
        }
    }
	else if (name == "rgbMonitor")
    {
        if (rgbMonitor)
        {
            rgbMonitor->removeObserver(this, CANVAS_MOUSE_DID_CHANGE);
            rgbMonitor->removeObserver(this, CANVAS_DID_COPY);
        }
        rgbMonitor = ref;
        if (rgbMonitor)
        {
            rgbMonitor->addObserver(this, CANVAS_MOUSE_DID_CHANGE);
            rgbMonitor->addObserver(this, CANVAS_DID_COPY);
            
            CanvasCaptureMode captureMode = CANVAS_CAPTURE_ON_MOUSE_CLICK;
            rgbMonitor->postMessage(this, CANVAS_SET_CAPTUREMODE, &captureMode);
        }
    }
    else
        return false;
    
    return true;
}

bool AppleIIIVideo::setData(string name, OEData *data)
{
    if (name == "characterSet")
        characterSet.swap(*data);
    else
        return false;
    
    return true;
}

bool AppleIIIVideo::getData(string name, OEData **data)
{
    if (name == "characterSet")
        *data = &characterSet;
    else
        return false;
    
    return true;
}

bool AppleIIIVideo::init()
{
    OECheckComponent(controlBus);
    OECheckComponent(systemControl);
    OECheckComponent(vram);
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    systemControl->postMessage(this, APPLEIII_GET_APPLEIIMODE, &appleIIMode);
    
    OEData *data;
    vram->postMessage(this, RAM_GET_DATA, &data);
    vramp = &data->front();
    
    update();
    
    return true;
}

void AppleIIIVideo::update()
{
    if (monitorConnected != (monochromeMonitor || compositeMonitor || rgbMonitor))
    {
        monitorConnected = (monochromeMonitor || compositeMonitor || rgbMonitor);
        
        postNotification(this, APPLEII_MONITOR_DID_CHANGE, &monitorConnected);
    }
    
    updateVideoEnabled();
}

bool AppleIIIVideo::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case APPLEII_REFRESH_VIDEO:
            refreshVideo();
            
            return true;
            
        case APPLEII_READ_FLOATINGBUS:
            *((OEChar *)data) = 0;
            
            return true;
            
        case APPLEII_IS_VBL:
            *((bool *)data) = (currentTimer == TIMER_VSYNC);
            
            return true;
            
        case APPLEII_IS_COLORKILLER_ENABLED:
            *((bool *) data) = colorKiller;
            
            break;
            
        case APPLEII_ASSERT_VIDEOINHIBIT:
            videoInhibitCount++;
            
            if (videoInhibitCount == 1)
            {
                bool videoInhibit = true;
                
                postNotification(this, APPLEII_VIDEOINHIBIT_DID_CHANGE, &videoInhibit);
                
                updateVideoEnabled();
            }
            
            return true;
            
        case APPLEII_CLEAR_VIDEOINHIBIT:
            videoInhibitCount--;
            
            if (!videoInhibitCount)
            {
                bool videoInhibit = false;
                
                postNotification(this, APPLEII_VIDEOINHIBIT_DID_CHANGE, &videoInhibit);
                
                updateVideoEnabled();
            }
            
            return true;
            
        case APPLEII_IS_VIDEO_INHIBITED:
            *((bool *)data) = videoInhibitCount;
            
            return true;
            
        case APPLEII_IS_MONITOR_CONNECTED:
            *((bool *) data) = monitorConnected;
            
            break;
            
        case APPLEIII_IS_BL:
        {
            OEIntPoint count = getCount();
            
            *((bool *)data) = ((count.y & 0xc0) != 0xc0) || (count.x);
            
            return true;
        }
        case APPLEIII_UPDATE_CHARACTERSET:
            return updateCharacterSet();
            
        default:
            if (monochromeMonitor)
                monochromeMonitor->postMessage(sender, message, data);
            if (compositeMonitor)
                compositeMonitor->postMessage(sender, message, data);
            if (rgbMonitor)
                rgbMonitor->postMessage(sender, message, data);
            
            return true;
    }
    
    return false;
}

void AppleIIIVideo::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
                powerState = *((ControlBusPowerState *)data);
                
                if (powerState == CONTROLBUS_POWERSTATE_OFF)
                {
                    setMode(MODE_TEXT, false);
                    setMode(MODE_MIXED, false);
                    setMode(MODE_PAGE2, false);
                    setMode(MODE_HIRES, false);
                }
                
                updateVideoEnabled();
                
                break;
                
            case CONTROLBUS_TIMER_DID_FIRE:
                scheduleNextTimer(*((OESLong *)data));
                
                break;
        }
    }
    else if ((sender == monochromeMonitor) ||
             (sender == compositeMonitor) ||
             (sender == rgbMonitor))
    {
        switch (notification)
        {
            case CANVAS_MOUSE_DID_CHANGE:
                postNotification(this, notification, data);
                
                break;
                
            case CANVAS_DID_COPY:
                postNotification(sender, notification, data);
                
                copy((wstring *)data);
                
                break;
        }
    }
    if (sender == systemControl)
    {
        appleIIMode = *((bool *)data);
        
        refreshVideo();
        
        configureDraw();
    }
    else
        // Refresh notification from VRAM
        refreshVideo();
}

OEChar AppleIIIVideo::read(OEAddress address)
{
    write(address, 0);
    
	return readFloatingBus();
}

void AppleIIIVideo::write(OEAddress address, OEChar value)
{
    switch (address & 0x7f)
    {
        case 0x50: case 0x51:
            setMode(MODE_TEXT, address & 0x1);
            
            break;
            
        case 0x52: case 0x53:
            setMode(MODE_MIXED, address & 0x1);
            
            break;
            
        case 0x54: case 0x55:
            setMode(MODE_PAGE2, address & 0x1);
            
            break;
            
        case 0x56: case 0x57:
            setMode(MODE_HIRES, address & 0x1);
            
            break;
    }
}

void AppleIIIVideo::initOffsets()
{
    textOffset.resize(BLOCK_HEIGHT * CELL_HEIGHT);
    
    for (OEInt y = 0; y < BLOCK_HEIGHT * CELL_HEIGHT; y++)
        textOffset[y] = (y >> 6) * BLOCK_WIDTH + ((y >> 3) & 0x7) * 0x80;
    
    hiresOffset.resize(BLOCK_HEIGHT * CELL_HEIGHT);
    
    for (OEInt y = 0; y < BLOCK_HEIGHT * CELL_HEIGHT; y++)
        hiresOffset[y] = (y >> 6) * BLOCK_WIDTH + ((y >> 3) & 0x7) * 0x80 + (y & 0x7) * 0x400;
}

bool AppleIIIVideo::updateCharacterSet()
{
    OEInt screenHole[] = { 0x078, 0x0f8, 0x178, 0x1f8, 0x278, 0x2f8, 0x378, 0x3f8 };
    
    for (OEInt holeIndex = 0; holeIndex < 8; holeIndex++)
    {
        for (OEInt i = 0; i < 8; i++)
        {
            OEInt address = APPLEIII_SYSTEMBANKADDRESS + screenHole[holeIndex] + i;
            
            OEChar fontValue = vramp[address + 0x400];
            OEChar fontAddress = vramp[address + 0x800] & 0x7f;
            
            OEInt a = fontValue;
            fontValue = 0;
            for (OEInt j = 0; j < 7; j++)
                OESetBit(fontValue, (1 << j), OEGetBit(a, (1 << (6 - j))));
            
            characterSet[8 * fontAddress + (2 * (holeIndex & 3) + (i >= 4))] = fontValue;
        }
    }
    
//    loadTextFont("", &font);
    
    return true;
}

void AppleIIIVideo::buildHiresFont()
{
    hires40Font.resize(2 * FONT_CHARNUM * FONT_CHARWIDTH);
    hires80Font.resize(FONT_CHARNUM * FONT_CHARWIDTH);
    
    for (OEInt i = 0; i < 2 * FONT_CHARNUM; i++)
    {
        OEChar value = (i & 0x7f) << 1 | (i >> 8);
        
        for (OEInt x = 0; x < FONT_CHARWIDTH; x++)
        {
            bool bit;
            
            bit = (value >> ((x + 2) >> 1)) & 0x1;
            hires40Font[i * FONT_CHARWIDTH + x] = bit ? 0xff : 0x00;
            
            bit = (value >> x) & 0x1;
            hires80Font[i * FONT_CHARWIDTH + x] = bit ? 0xff : 0x00;
        }
    }
}

void AppleIIIVideo::configureDraw()
{
    bool newColorKiller;
    
    bool page = OEGetBit(mode, MODE_PAGE2);
    
    if (appleIIMode)
    {
        newColorKiller = OEGetBit(mode, MODE_TEXT) || OEGetBit(mode, MODE_HIRES);
        
        if (OEGetBit(mode, MODE_TEXT) ||
            ((currentTimer == TIMER_DISPLAYEND) &&
             OEGetBit(mode, MODE_MIXED)))
        {
            draw = &AppleIIIVideo::drawText40MLine;
            drawMemory1 = vramp + APPLEIII_SYSTEMBANK * 0x8000 + 0x400 + 0x400 * page;
            drawFont = (OEChar *)&text40Font.front();
            
            drawFont += flash * FONT_SIZE;
        }
        else if (!OEGetBit(mode, MODE_HIRES))
        {
            draw = &AppleIIIVideo::drawLoresLine;
            drawMemory1 = vramp + APPLEIII_SYSTEMBANK * 0x8000 + 0x400 + 0x400 * page;
            drawFont = (OEChar *)&loresFont.front();
        }
        else
        {
            draw = &AppleIIIVideo::drawHires40MLine;
            drawMemory1 = vramp + 0x2000 * page;
            drawFont = (OEChar *)&hires40Font.front();
        }
    }
    else
    {
        switch (mode & (MODE_HIRES | MODE_MIXED | MODE_TEXT))
        {
            case 0:
                // Text 40 B/W
                newColorKiller = true;
                
                draw = &AppleIIIVideo::drawText40MLine;
                drawMemory1 = vramp + APPLEIII_SYSTEMBANK * 0x8000 + 0x400 + 0x400 * page;
                drawFont = (OEChar *)&text40Font.front();
                drawFont += flash * FONT_SIZE;
                
                break;
                
            case MODE_TEXT:
                // Text 40 color
                newColorKiller = false;
                
                draw = &AppleIIIVideo::drawText40CLine;
                drawMemory1 = vramp + APPLEIII_SYSTEMBANK * 0x8000 + 0x400 + 0x400 * page;
                drawMemory2 = vramp + APPLEIII_SYSTEMBANK * 0x8000 + 0x400 + 0x400 * !page;
                drawFont = (OEChar *)&text40Font.front();
                drawFont += flash * FONT_SIZE;
                
                break;
                
            case MODE_MIXED:
            case MODE_MIXED | MODE_TEXT:
                // Text 80 B/W
                newColorKiller = true;
                
                draw = &AppleIIIVideo::drawText80Line;
                drawMemory1 = vramp + APPLEIII_SYSTEMBANK * 0x8000 + 0x400 + 0x400 * page;
                drawMemory2 = vramp + APPLEIII_SYSTEMBANK * 0x8000 + 0x400 + 0x400 * !page;
                drawFont = (OEChar *)&text80Font.front();
                drawFont += flash * FONT_SIZE;
                
                break;
                
            case MODE_HIRES:
                // Apple II hires
                newColorKiller = true;
                
                draw = &AppleIIIVideo::drawHires40MLine;
                drawMemory1 = vramp + 0x2000 * page;
                drawFont = (OEChar *)&hires40Font.front();
                
                break;
                
            case MODE_HIRES | MODE_TEXT:
                // Apple III hires color
                newColorKiller = false;
                
                draw = &AppleIIIVideo::drawHires40CLine;
                drawMemory1 = vramp + 0x0000 + 0x4000 * page;
                drawMemory1 = vramp + 0x2000 + 0x4000 * !page;
                drawFont = (OEChar *)&hires40Font.front();
                
                break;
                
            case MODE_HIRES | MODE_MIXED:
                // Apple III super hires color
                newColorKiller = true;
                
                draw = &AppleIIIVideo::drawHires80Line;
                drawMemory1 = vramp + 0x0000 + 0x4000 * page;
                drawMemory2 = vramp + 0x2000 + 0x4000 * !page;
                drawFont = (OEChar *)&hires80Font.front();
                
                break;
                
            case MODE_HIRES | MODE_MIXED | MODE_TEXT:
                // Apple III 140x192 color
                newColorKiller = false;
                
                draw = &AppleIIIVideo::drawHires140Line;
                drawMemory1 = vramp + 0x0000 + 0x4000 * page;
                drawMemory2 = vramp + 0x2000 + 0x4000 * !page;
                drawFont = (OEChar *)&hires80Font.front();
                
                break;
        }
    }
}

// Copy a 14-pixel segment
#define copy40Segment(d,s) \
*((OELong *)(d + 0)) = *((OELong *)(s + 0));\
*((OEInt *)(d + 8)) = *((OEInt *)(s + 8));\
*((OEShort *)(d + 12)) = *((OEShort *)(s + 12));

// Copy an 8-pixel segment
#define copy80Segment(d,s) \
*((OELong *)(d + 0)) = *((OELong *)(s + 0));\
*((OEInt *)(d + 8)) = *((OEInt *)(s + 8));\
*((OEShort *)(d + 12)) = *((OEShort *)(s + 12));

void AppleIIIVideo::drawText40MLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = textOffset[y];
    OEChar *p = framebufferp + y * FRAMEBUFFER_WIDTH + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH)
    {
        OEChar *m = (drawFont + (y & 0x7) * FONT_CHARWIDTH +
                     drawMemory1[memoryOffset + x] * FONT_CHARSIZE);
        
        copy40Segment(p, m);
    }
}

void AppleIIIVideo::drawText40CLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = textOffset[y];
    OEChar *p = framebufferp + y * FRAMEBUFFER_WIDTH + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH)
    {
        OEChar *m = (drawFont + (y & 0x7) * FONT_CHARWIDTH +
                     drawMemory1[memoryOffset + x] * FONT_CHARSIZE);
        
        copy40Segment(p, m);
    }
}

void AppleIIIVideo::drawText80Line(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = textOffset[y];
    OEChar *p = framebufferp + y * FRAMEBUFFER_WIDTH + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH / 2)
    {
        OEChar *m = (drawFont + (y & 0x7) * FONT_CHARWIDTH +
                     drawMemory1[memoryOffset + x] * FONT_CHARSIZE);
        
        copy80Segment(p, m);
        
        p += CELL_WIDTH / 2;
        
        m = (drawFont + (y & 0x7) * FONT_CHARWIDTH +
             drawMemory2[memoryOffset + x] * FONT_CHARSIZE);
        
        copy80Segment(p, m);
    }
}

void AppleIIIVideo::drawLoresLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = textOffset[y];
    OEChar *p = framebufferp + y * FRAMEBUFFER_WIDTH + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH)
    {
        OEChar *m = (drawFont + (y & 0x7) * FONT_CHARWIDTH +
                     drawMemory1[memoryOffset + x] * FONT_CHARSIZE +
                     (x & 1) * FONT_CHARSIZE);
        
        copy40Segment(p, m);
    }
}

void AppleIIIVideo::drawHires40MLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = hiresOffset[y];
    OEChar *p = framebufferp + y * FRAMEBUFFER_WIDTH + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH)
    {
        OEInt offset = memoryOffset + x;
        
        OEInt i = drawMemory1[offset];
        OEChar *m = drawFont + i * FONT_CHARWIDTH;
        
        copy40Segment(p, m);
    }
}

void AppleIIIVideo::drawHires40CLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = hiresOffset[y];
    OEChar *p = framebufferp + y * FRAMEBUFFER_WIDTH + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH)
    {
        OEInt offset = memoryOffset + x;
        
        OEInt i = drawMemory1[offset];
        OEChar *m = drawFont + i * FONT_CHARWIDTH;
        
        copy40Segment(p, m);
    }
}

void AppleIIIVideo::drawHires80Line(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = hiresOffset[y];
    OEChar *p = framebufferp + y * FRAMEBUFFER_WIDTH + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH / 2)
    {
        OEChar *m = drawFont + drawMemory1[memoryOffset + x] * FONT_CHARWIDTH;
        
        copy80Segment(p, m);
        
        p += CELL_WIDTH / 2;
        
        m = drawFont + drawMemory2[memoryOffset + x] * FONT_CHARWIDTH;
        
        copy80Segment(p, m);
    }
}

void AppleIIIVideo::drawHires140Line(OESInt y, OESInt x0, OESInt x1)
{
}

void AppleIIIVideo::updateVideo()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    OEInt deltaCycles = (OEInt) (cycles - lastCycles);
    
    OEInt cycleNum = min(pendingCycles, deltaCycles);
    
    if (cycleNum)
    {
        pendingCycles -= cycleNum;
        
        if (videoEnabled)
        {
            OEInt segmentStart = (OEInt) (lastCycles - frameStart);
            
            OEIntPoint p0 = pos[segmentStart];
            OEIntPoint p1 = pos[segmentStart + cycleNum];
            
            if (p0.y == p1.y)
                (this->*draw)(p0.y, p0.x, p1.x);
            else
            {
                (this->*draw)(p0.y, p0.x, HORIZ_DISPLAY);
                
                for (OESInt i = (p0.y + 1); i < p1.y; i++)
                    (this->*draw)(i, 0, HORIZ_DISPLAY);
                
                (this->*draw)(p1.y, 0, p1.x);
            }
            
            framebufferModified = true;
        }
    }
    
    lastCycles = cycles;
}

bool AppleIIIVideo::loadTextFont(string name, OEData *data)
{
    return false;
}

void AppleIIIVideo::refreshVideo()
{
}

void AppleIIIVideo::buildLoresFont()
{
    
}

OEChar AppleIIIVideo::readFloatingBus()
{
    return 0;
}

void AppleIIIVideo::copy(wstring *s)
{
    
}

void AppleIIIVideo::setMode(OEInt mask, bool value)
{
    
}

OEIntPoint AppleIIIVideo::getCount()
{
    OEIntPoint a;
    
    a.x = 0;
    a.y = 0;
    
    return a;
}

void AppleIIIVideo::scheduleNextTimer(OESLong cycles)
{
    
}

void AppleIIIVideo::updateVideoEnabled()
{
    
}

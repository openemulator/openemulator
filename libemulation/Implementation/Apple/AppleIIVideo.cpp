
/**
 * libemulator
 * Apple II Video
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates Apple II video
 */

#include <math.h>

#include "AppleIIVideo.h"

#include "DeviceInterface.h"
#include "MemoryInterface.h"

#define HORIZ_START     16
#define HORIZ_BLANK     (9 + HORIZ_START)
#define HORIZ_DISPLAY   40
#define HORIZ_TOTAL     (HORIZ_BLANK + HORIZ_DISPLAY)

#define VERT_NTSC_START 38
#define VERT_PAL_START  48
#define VERT_DISPLAY    192

#define CELL_WIDTH      14
#define CELL_HEIGHT     8

#define BLOCK_WIDTH     HORIZ_DISPLAY
#define BLOCK_HEIGHT    (VERT_DISPLAY / CELL_HEIGHT)

#define FONT_CHARNUM    0x100
#define FONT_CHARWIDTH  16
#define FONT_CHARHEIGHT 8
#define FONT_CHARSIZE   (FONT_CHARWIDTH * FONT_CHARHEIGHT)
#define FONT_SIZE       (FONT_CHARNUM * FONT_CHARSIZE)

#define MODE_TEXT       (1 << 0)
#define MODE_MIXED      (1 << 1)
#define MODE_PAGE2      (1 << 2)
#define MODE_HIRES      (1 << 3)

AppleIIVideo::AppleIIVideo()
{
    controlBus = NULL;
    gamePort = NULL;
    monitor = NULL;
    
    model = APPLEII_MODELIIE;
    tvSystem = APPLEII_NTSC;
    characterSet = "Standard";
    flashFrameNum = 16;
    mode = 0;
    
    revisionUpdated = true;
    tvSystemUpdated = true;
    
    initOffsets();
    
    vram0000 = NULL;
    vram0000Offset = 0;
    vram1000 = NULL;
    vram1000Offset = 0;
    vram2000 = NULL;
    vram2000Offset = 0;
    vram4000 = NULL;
    vram4000Offset = 0;
    
    dummyMemory.resize(0x2000);
    
    for (OEInt page = 0; page < 2; page++)
    {
        textMemory[page] = &dummyMemory.front();
        hblMemory[page] = &dummyMemory.front();
        hiresMemory[page] = &dummyMemory.front();
    }
    
    videoEnabled = false;
    colorKiller = true;
    
    buildLoresFont();
    
    image.setSampleRate(NTSC_4FSC);
    image.setFormat(OEIMAGE_LUMINANCE);
    imageModified = false;
    
    frameStart = 0;
    frameCycleNum = 0;
    
    currentTimer = APPLEII_TIMER_VSYNC;
    lastCycles = 0;
    pendingCycles = 0;
    
    flash = false;
    flashCount = 0;
    
    powerState = CONTROLBUS_POWERSTATE_ON;
    an2 = false;
    monitorCaptured = false;
}

bool AppleIIVideo::setValue(string name, string value)
{
	if (name == "model")
    {
        if (value == "II")
            model = APPLEII_MODELII;
        else if (value == "II j-plus")
            model = APPLEII_MODELIIJPLUS;
        else if (value == "III")
            model = APPLEII_MODELIII;
        else if (value == "IIe")
            model = APPLEII_MODELIIE;
    }
	else if (name == "revision")
    {
        revision = getOEInt(value);
        
        revisionUpdated = true;
    }
	else if (name == "tvSystem")
    {
        if (value == "NTSC")
            tvSystem = APPLEII_NTSC;
        else if (value == "PAL")
            tvSystem = APPLEII_PAL;
        
        tvSystemUpdated = true;
    }
	else if (name == "characterSet")
		characterSet = value;
	else if (name == "flashFrameNum")
		flashFrameNum = getOEInt(value);
	else if (name == "text")
        OESetBit(mode, MODE_TEXT, getOEInt(value));
	else if (name == "mixed")
        OESetBit(mode, MODE_MIXED, getOEInt(value));
	else if (name == "page2")
        OESetBit(mode, MODE_PAGE2, getOEInt(value));
	else if (name == "hires")
        OESetBit(mode, MODE_HIRES, getOEInt(value));
	else if (name == "vram0000Offset")
        vram0000Offset = getOEInt(value);
	else if (name == "vram1000Offset")
        vram1000Offset = getOEInt(value);
	else if (name == "vram2000Offset")
        vram2000Offset = getOEInt(value);
	else if (name == "vram4000Offset")
        vram4000Offset = getOEInt(value);
    else
        return false;
	
	return true;
}

bool AppleIIVideo::getValue(string name, string& value)
{
    if (name == "revision")
        value = getString(revision);
	else if (name == "tvSystem")
    {
        switch (tvSystem)
        {
            case APPLEII_NTSC:
                value = "NTSC";
                
                break;
                
            case APPLEII_PAL:
                value = "PAL";
                
                break;
        }
    }
	else if (name == "characterSet")
		value = characterSet;
	else if (name == "flashFrameNum")
		value = getString(flashFrameNum);
	else if (name == "text")
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

bool AppleIIVideo::setRef(string name, OEComponent *ref)
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
    else if (name == "gamePort")
    {
        if (gamePort)
            gamePort->removeObserver(this, APPLEII_AN2_DID_CHANGE);
        gamePort = ref;
        if (gamePort)
            gamePort->addObserver(this, APPLEII_AN2_DID_CHANGE);
    }
	else if (name == "monitor")
    {
        if (monitor)
        {
            monitor->removeObserver(this, CANVAS_MOUSE_DID_CHANGE);
            monitor->removeObserver(this, CANVAS_DID_COPY);
        }
        monitor = ref;
        if (monitor)
        {
            monitor->addObserver(this, CANVAS_MOUSE_DID_CHANGE);
            monitor->addObserver(this, CANVAS_DID_COPY);
        }
        
        bool monitorConnected = (monitor != NULL);
        
        postNotification(this, APPLEII_MONITOR_DID_CHANGE, &monitorConnected);
    }
	else if (name == "vram0000")
        vram0000 = ref;
	else if (name == "vram1000")
        vram1000 = ref;
	else if (name == "vram2000")
        vram2000 = ref;
	else if (name == "vram4000")
        vram4000 = ref;
    else
		return false;
	
	return true;
}

bool AppleIIVideo::setData(string name, OEData *data)
{
    if (name.substr(0, 4) == "font")
        loadTextFont(name.substr(4), data);
    else
        return false;
    
    return true;
}

bool AppleIIVideo::init()
{
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    OEData *data;
    
    if (vram0000)
    {
        vram0000->postMessage(this, RAM_GET_DATA, &data);
        
        textMemory[0] = &data->front() + vram0000Offset + 0x400;
        textMemory[1] = &data->front() + vram0000Offset + 0x800;
    }
    
    if (vram1000)
    {
        vram1000->postMessage(this, RAM_GET_DATA, &data);
        
        hblMemory[0] = &data->front() + vram1000Offset + 0x400;
        hblMemory[1] = &data->front() + vram1000Offset + 0x800;
    }

    if (vram2000)
    {
        vram2000->postMessage(this, RAM_GET_DATA, &data);
        
        hiresMemory[0] = &data->front() + vram2000Offset;
    }

    if (vram4000)
    {
        vram4000->postMessage(this, RAM_GET_DATA, &data);
        
        hiresMemory[1] = &data->front() + vram4000Offset;
    }
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    if (gamePort)
        gamePort->postMessage(this, APPLEII_GET_AN2, &an2);
    
    update();
    
    return true;
}

void AppleIIVideo::update()
{
    if (revisionUpdated)
    {
        buildHiresFont();
        configureDraw();
        
        revisionUpdated = false;
    }
    
    if (tvSystemUpdated)
    {
        updateTiming();
        
        tvSystemUpdated = false;
    }
    
    if (monitor)
    {
        CanvasCaptureMode captureMode = CANVAS_CAPTURE_ON_MOUSE_CLICK;
        
        monitor->postMessage(this, CANVAS_SET_CAPTUREMODE, &captureMode);
    }
    
    updateVideoEnabled();
}

bool AppleIIVideo::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case APPLEII_REFRESH_VIDEO:
            refreshVideo();
            
            return true;
            
        case APPLEII_READ_FLOATINGBUS:
            *((OEChar *)data) = readFloatingBus();
            
            return true;
            
        case APPLEII_REQUEST_MONITOR:
            if (monitorCaptured)
                return false;
            
            monitorCaptured = true;
            
            updateVideoEnabled();
            
            return true;
            
        case APPLEII_RELEASE_MONITOR:
            monitorCaptured = false;
            
            updateVideoEnabled();
            
            return true;
            
        case APPLEII_IS_MONITOR_CONNECTED:
            *((bool *) data) = (monitor != NULL);
            
            break;
            
        case APPLEII_IS_COLORKILLER_ENABLED:
            *((bool *) data) = colorKiller;
            
            break;
            
        default:
            if (monitor)
                return monitor->postMessage(sender, message, data);
            
            break;
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
                
                updateVideoEnabled();
                
                break;
                
            case CONTROLBUS_TIMER_DID_FIRE:
                scheduleNextTimer(*((OESLong *)data));
                
                break;
        }
    }
    else if (sender == gamePort)
    {
        an2 = *((bool *)data);
        
        refreshVideo();
        
        configureDraw();
    }
    else if (sender == monitor)
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
    else
        // Refresh notification from VRAM
        refreshVideo();
}

OEChar AppleIIVideo::read(OEAddress address)
{
    write(address, 0);
    
	return readFloatingBus();
}

void AppleIIVideo::write(OEAddress address, OEChar value)
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

void AppleIIVideo::initOffsets()
{
    textOffset.resize(BLOCK_HEIGHT * CELL_HEIGHT);
    
    for (OEInt y = 0; y < BLOCK_HEIGHT * CELL_HEIGHT; y++)
        textOffset[y] = (y >> 6) * BLOCK_WIDTH + ((y >> 3) & 0x7) * 0x80;
    
    hiresOffset.resize(BLOCK_HEIGHT * CELL_HEIGHT);
    
    for (OEInt y = 0; y < BLOCK_HEIGHT * CELL_HEIGHT; y++)
        hiresOffset[y] = (y >> 6) * BLOCK_WIDTH + ((y >> 3) & 0x7) * 0x80 + (y & 0x7) * 0x400;
}

bool AppleIIVideo::loadTextFont(string name, OEData *data)
{
    if (data->size() < (FONT_CHARNUM * FONT_CHARHEIGHT))
        return false;
    
    OEData theFont;
    theFont.resize(4 * FONT_SIZE);
    
    OEInt mask = (OEInt) getNextPowerOf2(data->size() / FONT_CHARHEIGHT) - 1;
    for (OEInt i = 0; i < 4 * FONT_CHARNUM; i++)
    {
        for (OEInt y = 0; y < FONT_CHARHEIGHT; y++)
        {
            OEInt ir = i;
            
            if (model == APPLEII_MODELIIJPLUS)
            {
                OESetBit(ir, 0x40, OEGetBit(i, 0x80));
                OESetBit(ir, 0x80, OEGetBit(i, 0x200));
            }
            
            OEChar value = (*data)[(ir & mask) * FONT_CHARHEIGHT + y];
            OESetBit(value, 0x80, (i & 0xc0) == 0x40);
            
            bool inv = !(OEGetBit(i, 0x80) || (OEGetBit(value, 0x80) && OEGetBit(i, 0x100)));
            
            for (OEInt x = 0; x < FONT_CHARWIDTH; x++)
            {
                bool bit = (value << (x >> 1)) & 0x40;
                
                theFont[(i * FONT_CHARHEIGHT + y) * FONT_CHARWIDTH + x] = (bit ^ inv) ? 0xff : 0x00;
            }
        }
    }
    
    textFont[name] = theFont;
    
    return true;
}

void AppleIIVideo::buildLoresFont()
{
    loresFont.resize(2 * FONT_SIZE);
    
    for (OEInt i = 0; i < 2 * FONT_CHARNUM; i++)
    {
        for (OEInt y = 0; y < FONT_CHARHEIGHT; y++)
        {
            OEInt value = (y < 4) ? i & 0xf : (i >> 4) & 0xf;
            value = (value << 12) | (value << 8) | (value << 4) | value;
            value >>= OEGetBit(i, 0x100) ? 2 : 0;
            
            for (OEInt x = 0; x < FONT_CHARWIDTH; x++)
            {
                loresFont[(i * FONT_CHARHEIGHT + y) * FONT_CHARWIDTH + x] = (value & 0x1) ? 0xff : 0x00;
                
                value >>= 1;
            }
        }
    }
}

void AppleIIVideo::buildHiresFont()
{
    hiresFont.resize(2 * FONT_CHARNUM * FONT_CHARWIDTH);
    
    for (OEInt i = 0; i < 2 * FONT_CHARNUM; i++)
    {
        OEChar value = (i & 0x7f) << 1 | (i >> 8);
        bool delay = (revision != 0) && (i & 0x80);
        
        for (OEInt x = 0; x < FONT_CHARWIDTH; x++)
        {
            bool bit = (value >> ((x + 2 - delay) >> 1)) & 0x1;
            
            hiresFont[i * FONT_CHARWIDTH + x] = bit ? 0xff : 0x00;
        }
    }
}

void AppleIIVideo::setMode(OEInt mask, bool value)
{
    OEInt newMode = mode;
    OESetBit(newMode, mask, value);
    
    if (mode != newMode)
    {
        mode = newMode;
        
        refreshVideo();
        
        configureDraw();
    }
}

void AppleIIVideo::configureDraw()
{
    bool newColorKiller = (revision != 0) && OEGetBit(mode, MODE_TEXT);
    
    if (colorKiller != newColorKiller)
    {
        colorKiller = newColorKiller;
        
        image.setSubcarrier(colorKiller ? 0 : NTSC_FSC);
        
        postNotification(this, APPLEII_COLORKILLER_DID_CHANGE, &colorKiller);
    }
    
    bool page = OEGetBit(mode, MODE_PAGE2);
    
    if (OEGetBit(mode, MODE_TEXT) ||
        ((currentTimer == APPLEII_TIMER_DISPLAYEND) && OEGetBit(mode, MODE_MIXED)))
    {
        draw = &AppleIIVideo::drawTextLine;
        drawMemory = textMemory[page];
        drawFont = (OEChar *)&textFont[characterSet].front();
        
        drawFont += ((an2 << 1) | flash) * FONT_SIZE;
    }
    else if (!OEGetBit(mode, MODE_HIRES))
    {
        draw = &AppleIIVideo::drawLoresLine;
        drawMemory = textMemory[page];
        drawFont = (OEChar *)&loresFont.front();
    }
    else
    {
        draw = &AppleIIVideo::drawHiresLine;
        drawMemory = hiresMemory[page];
        drawFont = (OEChar *)&hiresFont.front();
    }
}

// Copy a 14-pixel segment
#define copySegment(d,s) \
*((OELong *)(d + 0)) = *((OELong *)(s + 0));\
*((OEInt *)(d + 8)) = *((OEInt *)(s + 8));\
*((OEShort *)(d + 12)) = *((OEShort *)(s + 12));

void AppleIIVideo::drawTextLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = textOffset[y];
    OEChar *p = imagep + y * imageWidth + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH)
    {
        OEChar i = drawMemory[memoryOffset + x];
        OEChar *m = (drawFont + (y & 0x7) * FONT_CHARWIDTH +
                     i * FONT_CHARSIZE);
        
        copySegment(p, m);
    }
}

void AppleIIVideo::drawLoresLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = textOffset[y];
    OEChar *p = imagep + y * imageWidth + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH)
    {
        OEChar i = drawMemory[memoryOffset + x];
        OEChar *m = (drawFont + (y & 0x7) * FONT_CHARWIDTH +
                     i * FONT_CHARSIZE +
                     (x & 1) * FONT_SIZE);
        
        copySegment(p, m);
    }
}

void AppleIIVideo::drawHiresLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = hiresOffset[y];
    OEChar *p = imagep + y * imageWidth + x0 * CELL_WIDTH;
    
    for (OEInt x = x0; x < x1; x++, p += CELL_WIDTH)
    {
        OEInt offset = memoryOffset + x;
        OELong lastOffset = (offset & ~0x7f) | ((offset - 1) & 0x7f);
        
        OEInt i = drawMemory[offset] | ((drawMemory[lastOffset] & 0x40) << 2);
        OEChar *m = drawFont + i * FONT_CHARWIDTH;
        
        copySegment(p, m);
    }
}

// To-Do: Implement Apple IIe delay

void AppleIIVideo::updateVideoEnabled()
{
    bool newVideoEnabled = (monitor &&
                            !monitorCaptured &&
                            (powerState != CONTROLBUS_POWERSTATE_OFF));
    
    if (videoEnabled != newVideoEnabled)
    {
        videoEnabled = newVideoEnabled;
        
        if (monitor)
        {
            if (!videoEnabled)
            {
                image.fill(OEColor());
                
                monitor->postMessage(this, CANVAS_CLEAR, NULL);
            }
            else
                refreshVideo();
        }
    }
}

void AppleIIVideo::refreshVideo()
{
    updateVideo();
    
    pendingCycles = frameCycleNum;
}

void AppleIIVideo::updateVideo()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    OEInt deltaCycles = (OEInt) (cycles - lastCycles);
    
    OEInt cycleNum = min(pendingCycles, deltaCycles);
    
    if (cycleNum && videoEnabled)
    {
        pendingCycles -= cycleNum;
        
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
        
        imageModified = true;
    }
    
    lastCycles = cycles;
}

void AppleIIVideo::updateTiming()
{
    // Update parameters
    float clockFrequency;
    OERect visibleRect, displayRect;
    
    if (tvSystem == APPLEII_NTSC)
    {
        clockFrequency = NTSC_4FSC * HORIZ_TOTAL / 912;
        
        visibleRect = OEMakeRect(clockFrequency * NTSC_HSTART, NTSC_VSTART,
                                 clockFrequency *  NTSC_HLENGTH, NTSC_VLENGTH);
        displayRect = OEMakeRect(HORIZ_START, VERT_NTSC_START, HORIZ_DISPLAY, VERT_DISPLAY);
        
        vertTotal = NTSC_VTOTAL;
    }
    else if (tvSystem == APPLEII_PAL)
    {
        clockFrequency = 14250450.0F * HORIZ_TOTAL / 912;
        
        visibleRect = OEMakeRect(clockFrequency * PAL_HSTART, PAL_VSTART,
                                 clockFrequency * PAL_HLENGTH, PAL_VLENGTH);
        displayRect = OEMakeRect(HORIZ_START, VERT_PAL_START, HORIZ_DISPLAY, VERT_DISPLAY);
        
        vertTotal = PAL_VTOTAL;
    }
    
    controlBus->postMessage(this, CONTROLBUS_SET_CLOCKFREQUENCY, &clockFrequency);
    
    OESInt horizStart = OEMinX(displayRect);
    vertStart = OEMinY(displayRect);
    
    frameCycleNum = HORIZ_TOTAL * vertTotal;
    
    OESInt imageLeft = (OEInt) ((horizStart - OEMinX(visibleRect)) * CELL_WIDTH);
    
    // Update image
    image.setSize(OEIntegralSize(OEMakeSize(CELL_WIDTH * visibleRect.size.width,
                                            visibleRect.size.height)));
    imageWidth = image.getSize().width;
    
    imagep = image.getPixels();
    imagep += (OEInt) (vertStart - OEMinY(visibleRect)) * imageWidth + imageLeft;
    
    vector<float> colorBurst;
    colorBurst.push_back(2.0 * M_PI * (-33.0 / 360.0 + (imageLeft % 4) / 4.0));
    image.setColorBurst(colorBurst);
    
    // Update pos and count
    OEInt cycleNum = frameCycleNum + 16;
    
    pos.resize(cycleNum);
    count.resize(cycleNum);
    
    for (OEInt i = 0; i < cycleNum; i++)
    {
        OEPoint p = OEGetPosInRect(OEMakePoint(i % HORIZ_TOTAL, i / HORIZ_TOTAL),
                                   displayRect);
        
        pos[i].x = p.x - HORIZ_START;
        pos[i].y = p.y - vertStart;
        
        OESInt ci = i + (HORIZ_BLANK - HORIZ_START);
        OEPoint c = OEMakePoint(ci % HORIZ_TOTAL, ci / HORIZ_TOTAL);
        
        c.x = c.x ? (c.x + 0x40 - 1) : 0;
        c.y = c.y + 0x100 - vertStart;
        
        if (c.y < (0x200 - vertTotal))
            c.y += vertTotal;
        else if (c.y > 0x200)
            c.y -= vertTotal;
        
        count[i].x = c.x;
        count[i].y = c.y;
    }
    
    currentTimer = APPLEII_TIMER_VSYNC;
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &lastCycles);
    
    OEInt id = 0;
    controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, &id);
    
    scheduleNextTimer(0);
}

void AppleIIVideo::scheduleNextTimer(OESLong cycles)
{
    updateVideo();
    
    currentTimer++;
    if (currentTimer > APPLEII_TIMER_DISPLAYEND)
        currentTimer = APPLEII_TIMER_VSYNC;
    
    switch (currentTimer)
    {
        case APPLEII_TIMER_DISPLAYMIXED:
            if (imageModified)
            {
                imageModified = false;
                
                if (videoEnabled)
                    monitor->postMessage(this, CANVAS_POST_IMAGE, &image);
            }
            
            if (powerState == CONTROLBUS_POWERSTATE_ON)
            {
                flashCount++;
                
                if (flashCount >= flashFrameNum)
                {
                    flash = !flash;
                    flashCount = 0;
                    
                    refreshVideo();
                }
            }
            
            configureDraw();
            
            controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &frameStart);
            frameStart += cycles;
            
            cycles += (vertStart + VERT_DISPLAY - 32) * HORIZ_TOTAL;
            
            break;
            
        case APPLEII_TIMER_DISPLAYEND:
            configureDraw();
            
            cycles += 32 * HORIZ_TOTAL;
            
            break;
            
        case APPLEII_TIMER_VSYNC:
            cycles += (vertTotal - (vertStart + VERT_DISPLAY)) * HORIZ_TOTAL;
            
            break;
    }
    
    ControlBusTimer timer = { cycles, 0 };
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &timer);
}

OEIntPoint AppleIIVideo::getCount()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    return count[(size_t) (cycles - frameStart)];
}

OEChar AppleIIVideo::readFloatingBus()
{
    OEIntPoint count = getCount();
    
	bool mixed = OEGetBit(mode, MODE_MIXED) && ((count.y & 0xa0) == 0xa0);
	bool hires = OEGetBit(mode, MODE_HIRES) && !(OEGetBit(mode, MODE_TEXT) || mixed);
	bool page = OEGetBit(mode, MODE_PAGE2);
	
	OEInt address = count.x & 0x7;
	
    OEInt h5h4h3 = count.x & 0x38;
    
    OEInt v4v3 = count.y & 0xc0;
	OEInt v4v3v4v3 = (v4v3 >> 3) | (v4v3 >> 1);
	
    address |= (0x68 + h5h4h3 + v4v3v4v3) & 0x78;
	address |= (count.y & 0x38) << 4;
    
	if (hires)
    {
		address |= (count.y & 0x7) << 10;
        
        return hiresMemory[page][address];
	}
    else
    {
        // Apple II: set A12 on horizontal blanking
        if ((model != APPLEII_MODELIIE) && (count.x < 0x58))
            return hblMemory[page][address];
        
        return textMemory[page][address];
    }
}

void AppleIIVideo::copy(wstring *s)
{
    if (!videoEnabled || !OEGetBit(mode, MODE_TEXT))
        return;
    
    OEChar charMap[] = {0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x40, 0x60};
    
    bool page = OEGetBit(mode, MODE_PAGE2);
    
    OEChar *vp = textMemory[page];
    
    if (!vp)
        return;
    
    for (OEInt y = 0; y < VERT_DISPLAY; y += CELL_HEIGHT)
    {
        wstring line;
        
        for (OEInt x = 0; x < BLOCK_WIDTH; x++)
        {
            OEChar value = vp[textOffset[y] + x];
            
            // To-Do: Mousetext
            line += charMap[value >> 5] | (value & 0x1f);
        }
        
        line = rtrim(line);
        line += '\n';
        
        *s += line;
    }
}

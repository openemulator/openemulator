
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

#define VIDEO_HDISPLAY  40
#define VIDEO_HEND      9
#define VIDEO_HSTART    16
#define VIDEO_HBLANK    (VIDEO_HEND + VIDEO_HSTART)
#define VIDEO_HTOTAL    (VIDEO_HDISPLAY + VIDEO_HBLANK)

#define CELL_WIDTH      14
#define CELL_HEIGHT     8

#define BLOCK_WIDTH     VIDEO_HDISPLAY
#define BLOCK_HEIGHT    24

#define DISPLAY_WIDTH   (BLOCK_WIDTH * CELL_WIDTH)
#define DISPLAY_HEIGHT  (BLOCK_HEIGHT * CELL_HEIGHT)

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
    device = NULL;
    controlBus = NULL;
    memoryBus = NULL;
    gamePort = NULL;
    monitorDevice = NULL;
    monitor = NULL;
    
    model = APPLEII_MODELIIE;
    tvSystem = APPLEII_NTSC;
    characterSet = "Standard";
    flashFrameNum = 16;
    mode = 0;
    
    revisionUpdated = true;
    tvSystemUpdated = true;
    
    initOffsets();
    buildLoresFont();
    
    image.setSampleRate(NTSC_4FSC);
    image.setFormat(OEIMAGE_LUMINANCE);
    imageModified = false;
    
    videoEnabled = false;
    colorKiller = true;
    
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
    if (name == "device")
        device = ref;
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
    else if (name == "memoryBus")
    {
        if (memoryBus)
            memoryBus->removeObserver(this, APPLEII_VRAM_DID_CHANGE);
        memoryBus = ref;
        if (memoryBus)
            controlBus->addObserver(this, APPLEII_VRAM_DID_CHANGE);
    }
    else if (name == "gamePort")
    {
        if (gamePort)
            gamePort->removeObserver(this, APPLEII_AN2_DID_CHANGE);
        gamePort = ref;
        if (gamePort)
            gamePort->addObserver(this, APPLEII_AN2_DID_CHANGE);
    }
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
            monitor->removeObserver(this, CANVAS_MOUSE_DID_CHANGE);
            monitor->removeObserver(this, CANVAS_DID_COPY);
        }
        monitor = ref;
        if (monitor)
        {
            monitor->addObserver(this, CANVAS_MOUSE_DID_CHANGE);
            monitor->addObserver(this, CANVAS_DID_COPY);
        }
    }
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
    
    if (!memoryBus)
    {
        logMessage("memoryBus not connected");
        
        return false;
    }
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    memoryBus->postMessage(this, APPLEII_GET_VRAM, &vram);
    
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
    
    configureDraw();
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
            
        case APPLEII_GET_COLORKILLER:
            *((bool *) data) = colorKiller;
            
            break;
            
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
                
                break;
                
            case CONTROLBUS_TIMER_DID_FIRE:
                scheduleNextTimer(*((OESLong *)data));
                
                break;
        }
    }
    else if (sender == memoryBus)
        memoryBus->postMessage(this, APPLEII_GET_VRAM, &vram);
    else if (sender == gamePort)
    {
        an2 = *((bool *)data);
        
        refreshVideo();
        
        configureDraw();
    }
    else if (sender == monitorDevice)
        device->postNotification(sender, notification, data);
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
        
        postNotification(this, APPLEII_COLORKILLER_DID_CHANGE, &colorKiller);
        image.setSubcarrier(colorKiller ? 0 : NTSC_FSC);
    }
    
    bool page = OEGetBit(mode, MODE_PAGE2);
    
    if (OEGetBit(mode, MODE_TEXT) ||
        ((currentTimer == APPLEII_TIMER_DISPLAYEND) && OEGetBit(mode, MODE_MIXED)))
    {
        draw = &AppleIIVideo::drawTextLine;
        drawMemory = vram.textMain[page];
        drawFont = (OEChar *)&textFont[characterSet].front();
        
        drawFont += ((an2 << 1) | flash) * FONT_SIZE;
    }
    else if (!OEGetBit(mode, MODE_HIRES))
    {
        draw = &AppleIIVideo::drawLoresLine;
        drawMemory = vram.textMain[page];
        drawFont = (OEChar *)&loresFont.front();
    }
    else
    {
        draw = &AppleIIVideo::drawHiresLine;
        drawMemory = vram.hiresMain[page];
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
                            powerState != CONTROLBUS_POWERSTATE_OFF);
    
    if (videoEnabled != newVideoEnabled)
    {
        videoEnabled = newVideoEnabled;
        
        if (monitor)
        {
            if (!videoEnabled)
                monitor->postMessage(this, CANVAS_CLEAR, NULL);
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
    
    if (cycleNum)
    {
        pendingCycles -= cycleNum;
        
        OEInt segmentStart = (OEInt) (lastCycles - frameStart);
        
        OECount p0 = pos[segmentStart];
        OECount p1 = pos[segmentStart + cycleNum];
        
        if (p0.y == p1.y)
            (this->*draw)(p0.y, p0.x, p1.x);
        else
        {
            (this->*draw)(p0.y, p0.x, VIDEO_HDISPLAY);
            
            for (OESInt i = (p0.y + 1); i < p1.y; i++)
                (this->*draw)(i, 0, VIDEO_HDISPLAY);
            
            (this->*draw)(p1.y, 0, p1.x);
        }
        
        imageModified = true;
    }
    
    lastCycles = cycles;
}

void AppleIIVideo::updateTiming()
{
    // Update parameters
    float clockFrequency = 0;
    
    OERect visibleRect;
    
    if (tvSystem == APPLEII_NTSC)
    {
        clockFrequency = NTSC_4FSC * VIDEO_HTOTAL / 912;
        
        vertTotal = NTSC_VTOTAL;
        vertDisplayStart = 38;
        
        visibleRect = OEMakeRect((OEInt) (clockFrequency * CELL_WIDTH * NTSC_HSTART), NTSC_VSTART,
                                 (OEInt) (clockFrequency * CELL_WIDTH * NTSC_HLENGTH), NTSC_VLENGTH);
    }
    else if (tvSystem == APPLEII_PAL)
    {
        clockFrequency = 14250450.0 * VIDEO_HTOTAL / 912;
        
        vertTotal = PAL_VTOTAL;
        vertDisplayStart = 48;
        
        visibleRect = OEMakeRect((OEInt) (clockFrequency * CELL_WIDTH * PAL_HSTART), PAL_VSTART,
                                 (OEInt) (clockFrequency * CELL_WIDTH * PAL_HLENGTH), PAL_VLENGTH);
    }
    
    OERect displayRect = OEMakeRect(CELL_WIDTH * VIDEO_HSTART, vertDisplayStart,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT);
    
    frameCycleNum = VIDEO_HTOTAL * vertTotal;
    controlBus->postMessage(this, CONTROLBUS_SET_CLOCKFREQUENCY, &clockFrequency);
    
    // Resize image
    image.setSize(visibleRect.size);
    imageWidth = image.getSize().width;
    
    imagep = image.getPixels();
    imagep += (OEInt) ((OEMinY(displayRect) - OEMinY(visibleRect)) * imageWidth);
    imagep += (OEInt) (OEMinX(displayRect) - OEMinX(visibleRect));
    
    vector<float> colorBurst;
    colorBurst.push_back(2.0 * M_PI * (-33.0 / 360.0 + ((OEInt) OEMinX(visibleRect) % 4) / 4.0));
    
    image.setColorBurst(colorBurst);
    
    // Build pos and count data
    OEInt cycleNum = frameCycleNum + 16;
    
    pos.resize(cycleNum);
    count.resize(cycleNum);
    
    for (OEInt i = 0; i < cycleNum; i++)
    {
        OESInt sx = i % 65;
        OESInt sy = i / 65;
        
        pos[i].x = sx - VIDEO_HSTART;
        pos[i].y = sy - vertDisplayStart;
        
        if (pos[i].x < 0)
            pos[i].x = 0;
        if (pos[i].x > BLOCK_WIDTH)
            pos[i].x = BLOCK_WIDTH;
        
        if (pos[i].y < 0)
        {
            pos[i].x = 0;
            pos[i].y = 0;
        }
        if (pos[i].y >= DISPLAY_HEIGHT)
        {
            pos[i].x = BLOCK_WIDTH;
            pos[i].y = DISPLAY_HEIGHT - 1;
        }
        
        OESInt ci = i + VIDEO_HBLANK;
        OESInt cx = ci % 65;
        OESInt cy = ci / 65;
        
        count[i].x = cx ? (cx + 0x40 - 1) : 0;
        count[i].y = cy + 0x100 - vertDisplayStart;
        
        if (count[i].y < (0x200 - vertTotal))
            count[i].y += vertTotal;
        else if (count[i].y > 0x200)
            count[i].y -= vertTotal;
    }
    
    currentTimer = APPLEII_TIMER_VSYNC;
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &lastCycles);
    
    controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, this);
    
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
            
            cycles += (vertDisplayStart + DISPLAY_HEIGHT - 32) * VIDEO_HTOTAL;
            
            break;
            
        case APPLEII_TIMER_DISPLAYEND:
            configureDraw();
            
            cycles += 32 * VIDEO_HTOTAL;
            
            break;
            
        case APPLEII_TIMER_VSYNC:
            cycles += (vertTotal - (vertDisplayStart + DISPLAY_HEIGHT)) * VIDEO_HTOTAL;
            
            break;
    }
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
}

OECount AppleIIVideo::getCount()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    return count[(size_t) (cycles - frameStart)];
}

OEChar AppleIIVideo::readFloatingBus()
{
    OECount count = getCount();
    
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
        
        return vram.hiresMain[page][address];
	}
    else
    {
        // Apple II: set A12 on horizontal blanking
        if ((model != APPLEII_MODELIIE) && (count.x < 0x58))
            return vram.hbl[page][address];
        
        return vram.textMain[page][address];
    }
}

void AppleIIVideo::copy(wstring *s)
{
    if (!videoEnabled || !OEGetBit(mode, MODE_TEXT))
        return;
    
    OEChar charMap[] = {0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x40, 0x60};
    
    bool page = OEGetBit(mode, MODE_PAGE2);
    
    OEChar *vp = vram.textMain[page];
    
    if (!vp)
        return;
    
    for (OEInt y = 0; y < DISPLAY_HEIGHT; y += CELL_HEIGHT)
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

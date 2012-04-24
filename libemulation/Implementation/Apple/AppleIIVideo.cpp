
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

#define ACTIVE_HEIGHT       192
#define TERM_WIDTH          40
#define TERM_HEIGHT         24
#define CHAR_WIDTH          14
#define CHAR_HEIGHT         8

#define MAP_SIZE            0x100
#define MAP_SIZE_MASK       0xff
#define MAP_WIDTH           16
#define MAP_HEIGHT          8

#define MODE_TEXT       (1 << 0)
#define MODE_MIXED      (1 << 1)
#define MODE_PAGE2      (1 << 2)
#define MODE_HIRES      (1 << 3)

/*
 * Notes:
 *
 * Video and picture rects:
 *
 * +------------------+
 * |                  | 19 lines (NTSC) / 21 lines (PAL)
 * |  +-------------+ |
 * |  |             | |
 * |  |   Picture   | | 240 lines (NTSC) / 288 lines (PAL)
 * |  |             | |
 * |  +-------------+ |
 * |--+-------------+-| 3 lines
 * 
 *  ^
 *  | 16 samples HSYNC, 16 samples color carrier, 768 samples picture
 */

AppleIIVideo::AppleIIVideo()
{
    model = APPLEII_MODELIIE;
    tvSystem = APPLEII_NTSC;
    characterSet = "Standard";
    flashFrameNum = 16;
    mode = 0;
    
    device = NULL;
    controlBus = NULL;
    gamePort = NULL;
    monitorDevice = NULL;
    monitor = NULL;
    
    initPoints();
    initOffsets();
    initLoresMap();
    
    vector<float> colorBurst;
    colorBurst.push_back(M_PI * -33.0 / 180.0);
    
    image.setSampleRate(14318180);
    image.setFormat(OEIMAGE_LUMINANCE);
    image.setColorBurst(colorBurst);
    imageDidChange = true;
    
    frameStart = 0;
    currentTimer = APPLEII_TIMER_VSYNC;
    lastSegment = 0;
    pendingSegments = 0;
    
    flashActive = false;
    flashCount = flashFrameNum;
    
    powerState = CONTROLBUS_POWERSTATE_ON;
    
    isRevisionUpdated = true;
    isTVSystemUpdated = true;
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
        if (value == "Revision 0")
            revision = APPLEII_REVISION0;
        else if (value == "Revision 1+")
            revision = APPLEII_REVISION1;
        
        isRevisionUpdated = true;
    }
	else if (name == "tvSystem")
    {
        if (value == "NTSC")
            tvSystem = APPLEII_NTSC;
        else if (value == "PAL")
            tvSystem = APPLEII_PAL;
        
        isTVSystemUpdated = true;
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
    {
        switch (revision)
        {
            case APPLEII_REVISION0:
                value = "Revision 0";
                
                break;
                
            case APPLEII_REVISION1:
                value = "Revision 1+";
                
                break;
        }
    }
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
        memoryBus = ref;
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
        loadTextMap(name.substr(4), data);
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
    if (isRevisionUpdated)
    {
        updateHiresMap();
        
        isRevisionUpdated = false;
    }
    
    if (isTVSystemUpdated)
    {
        if (tvSystem == APPLEII_NTSC)
        {
            videoRect = OEMakeRect(0, 0, 912, 262);
            pictureRect = OEMakeRect(128, 19, 768, 240);
            activeRect = OEMakeRect(232, 38, 560, 192);
        }
        else if (tvSystem == APPLEII_PAL)
        {
            videoRect = OEMakeRect(0, 0, 912, 312);
            pictureRect = OEMakeRect(128, 21, 768, 288);
            activeRect = OEMakeRect(232, 48, 560, 192);
        }
        
        updateSegments();
        updateCounts();
        updateImage();
        updateClockFrequency();
        
        currentTimer = APPLEII_TIMER_VSYNC;
        lastSegment = 0;
        
        controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, this);
        
        scheduleNextTimer(0);
        
        isTVSystemUpdated = false;
    }
    
    if (monitor)
    {
        CanvasCaptureMode captureMode = CANVAS_CAPTUREMODE_CAPTURE_ON_MOUSE_CLICK;
        
        monitor->postMessage(this, CANVAS_SET_CAPTUREMODE, &captureMode);
    }
    
    updateVideo();
    
    updateRendererMap();
    
    updateRenderer();
    
    setNeedsDisplay();
}

bool AppleIIVideo::postMessage(OEComponent *sender, int message, void *data)
{
    switch (message)
    {
        case APPLEII_REFRESH_VIDEO:
            setNeedsDisplay();
            
            return true;
            
        case APPLEII_READ_FLOATINGBUS:
            *((OEChar *)data) = readFloatingBus();
            
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
                scheduleNextTimer(*((OESLong *)data));
                
                break;
        }
    }
    else if (sender == gamePort)
    {
        an2 = *((bool *)data);
        
        updateRendererMap();
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

void AppleIIVideo::updateSegments()
{
    OEInt cycleNum = 65 * OEHeight(videoRect) + 256;
    
    segment.resize(cycleNum);
    
    for (OEInt i = 0; i < cycleNum; i++)
    {
        OESInt xv = i % 65 - (65 - TERM_WIDTH);
        OESInt yv = i / 65 - OEMinY(activeRect);
        
        if (xv < 0)
            xv = 0;
        
        if (yv < 0)
        {
            xv = 0;
            yv = 0;
        }
        else if (yv >= ACTIVE_HEIGHT)
        {
            xv = 0;
            yv = 0;
        }
        
        segment[i] = xv + yv * TERM_WIDTH;
    }
}

void AppleIIVideo::initPoints()
{
    point.resize(TERM_WIDTH * ACTIVE_HEIGHT);
    
    for (OEInt i = 0; i < TERM_WIDTH * ACTIVE_HEIGHT; i++)
    {
        point[i].x = i % TERM_WIDTH;
        point[i].y = i / TERM_WIDTH;
    }
}

void AppleIIVideo::updateCounts()
{
    OEInt cycleNum = 65 * OEHeight(videoRect) + 16;
    
    count.resize(cycleNum);
    
    OEInt videoHeight = (int) OEHeight(videoRect);
    
    for (OEInt i = 0; i < cycleNum; i++)
    {
        OEInt x = i % 65;
        OEInt y = i / 65;
        
        count[i].x = x ? (x + 0x40 - 1) : 0;
        count[i].y = y + 0x100 - (int) OEMinY(activeRect);
        
        if (count[i].y < (0x200 - videoHeight))
            count[i].y += videoHeight;
        else if (count[i].y > 0x200)
            count[i].y -= videoHeight;
    }
}

void AppleIIVideo::initOffsets()
{
    textOffset.resize(TERM_HEIGHT);
    
    for (OEInt y = 0; y < TERM_HEIGHT; y++)
        textOffset[y] = (y >> 3) * TERM_WIDTH + (y & 0x7) * 0x80;
    
    hiresOffset.resize(TERM_HEIGHT * CHAR_HEIGHT);
    
    for (OEInt y = 0; y < TERM_HEIGHT * CHAR_HEIGHT; y++)
        hiresOffset[y] = (y >> 6) * TERM_WIDTH + ((y >> 3) & 0x7) * 0x80 + (y & 0x7) * 0x400;
}

void AppleIIVideo::loadTextMap(string name, OEData *data)
{
    if (data->size() < MAP_HEIGHT)
        return;
    
    OEData theMap;
    
    theMap.resize(4 * MAP_SIZE * MAP_HEIGHT * MAP_WIDTH);
    
    OEInt cMask = (int) getNextPowerOf2(data->size() / MAP_HEIGHT) - 1;
    
    for (OEInt c = 0; c < 4 * MAP_SIZE; c++)
    {
        for (OEInt y = 0; y < MAP_HEIGHT; y++)
        {
            OEInt cr = c;
            
            if (model == APPLEII_MODELIIJPLUS)
            {
                OESetBit(cr, 0x40, OEGetBit(c, 0x80));
                OESetBit(cr, 0x80, OEGetBit(c, 0x200));
            }
            
            OEChar byte = data->at((cr & cMask) * MAP_HEIGHT + y);
            
            OESetBit(byte, 0x80, (c & 0xc0) == 0x40);
            
            bool inv = !(OEGetBit(c, 0x80) || (OEGetBit(byte, 0x80) && OEGetBit(c, 0x100)));
            
            for (OEInt x = 0; x < MAP_WIDTH; x++)
            {
                bool b = (byte << (x >> 1)) & 0x40;
                
                theMap[(c * MAP_HEIGHT + y) * MAP_WIDTH + x] = (b ^ inv) ? 0xff : 0x00;
            }
        }
    }
    
    textMap[name] = theMap;
}

void AppleIIVideo::initLoresMap()
{
    loresMap.resize(2 * MAP_SIZE * MAP_HEIGHT * MAP_WIDTH);
    
    for (OEInt c = 0; c < 2 * MAP_SIZE; c++)
    {
        for (OEInt y = 0; y < MAP_HEIGHT; y++)
        {
            OEInt v = (y < 4) ? c & 0xf : (c >> 4) & 0xf;
            
            OEInt bitmap = (v << 12) | (v << 8) | (v << 4) | v;
            bitmap >>= OEGetBit(c, 0x100) ? 2 : 0;
            
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                loresMap[(c * MAP_HEIGHT + y) * MAP_WIDTH + x] = (bitmap & 0x1) ? 0xff : 0x00;
                
                bitmap >>= 1;
            }
        }
    }
}

void AppleIIVideo::updateHiresMap()
{
    hiresMap.resize(2 * MAP_SIZE * MAP_WIDTH);
    
    for (OEInt c = 0; c < 2 * MAP_SIZE; c++)
    {
        OEChar byte = (c & 0x7f) << 1 | (c >> 8);
        bool delay = (revision != APPLEII_REVISION0) && (c & 0x80);
        
        for (OEInt x = 0; x < MAP_WIDTH; x++)
        {
            bool b = (byte >> ((x + 2 - delay) >> 1)) & 0x1;
            
            hiresMap[c * MAP_WIDTH + x] = b ? 0xff : 0x00;
        }
    }
}

void AppleIIVideo::updateImage()
{
    image.setSize(pictureRect.size);
    
    rendererImage = image.getPixels();
    rendererImage += (int) ((OEMinY(activeRect) - OEMinY(pictureRect)) * OEWidth(pictureRect));
    rendererImage += (int) (OEMinX(activeRect) - OEMinX(pictureRect));
}

void AppleIIVideo::updateClockFrequency()
{
    float clockFrequency = 0;
    
    if (tvSystem == APPLEII_NTSC)
        clockFrequency = 14318180.0 * 65 / 912;
    else if (tvSystem == APPLEII_PAL)
        clockFrequency = 14250450.0 * 65 / 912;
    
    controlBus->postMessage(this, CONTROLBUS_SET_CLOCKFREQUENCY, &clockFrequency);
}

void AppleIIVideo::updateRendererMap()
{
    rendererTextMap = (OEChar *)&textMap[characterSet].front();
    
    if (an2)
        rendererTextMap += 2 * MAP_SIZE * MAP_WIDTH * MAP_HEIGHT;
    if (flashActive)
        rendererTextMap += MAP_SIZE * MAP_WIDTH * MAP_HEIGHT;
    
    rendererLoresMap = (OEChar *)&loresMap.front();
    
    rendererHiresMap = (OEChar *)&hiresMap.front();
}

void AppleIIVideo::updateRenderer()
{
    if ((revision == APPLEII_REVISION0) || !OEGetBit(mode, MODE_TEXT))
        image.setSubcarrier(3579545);
    else
        image.setSubcarrier(0);
    
    OEInt page = OEGetBit(mode, MODE_PAGE2) ? 1 : 0;
    
    rendererTextMemory = vram.textMain[page];
    rendererHiresMemory = vram.hiresMain[page];
    
    switch (currentTimer)
    {
        case APPLEII_TIMER_MIXED:
            if (OEGetBit(mode, MODE_TEXT))
                renderer = APPLEII_RENDERER_TEXT;
            else if (!OEGetBit(mode, MODE_HIRES))
                renderer = APPLEII_RENDERER_LORES;
            else
                renderer = APPLEII_RENDERER_HIRES;
            
            break;
            
        case APPLEII_TIMER_ACTIVEEND:
            if (OEGetBit(mode, MODE_TEXT) ||
                OEGetBit(mode, MODE_MIXED))
                renderer = APPLEII_RENDERER_TEXT;
            else if (!OEGetBit(mode, MODE_HIRES))
                renderer = APPLEII_RENDERER_LORES;
            else
                renderer = APPLEII_RENDERER_HIRES;
            
            break;
            
        default:
            break;
    }
}

void AppleIIVideo::setMode(OEInt mask, bool value)
{
    OEInt oldMode = mode;
    
    OESetBit(mode, mask, value);
    
    if (mode != oldMode)
    {
        updateVideo();
        
        updateRenderer();
        
        setNeedsDisplay();
    }
}

void AppleIIVideo::updateVideo()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    OEInt currentSegment = segment[(size_t) (cycles - frameStart)];
    
    OESInt deltaSegment = currentSegment - lastSegment;
    
    if (deltaSegment < 0)
        deltaSegment += ACTIVE_HEIGHT * TERM_WIDTH;
    
    OEInt segmentNum = min(deltaSegment, pendingSegments);
    
    if (segmentNum)
    {
        pendingSegments -= segmentNum;
        
        OEInt thisSegment = lastSegment + segmentNum;
        
        AppleIIVideoPoint p0 = point[lastSegment];
        AppleIIVideoPoint p1 = point[thisSegment - 1];
        
        if (p0.y == p1.y)
            drawVideoLine(p0.y, p0.x, p1.x);
        else
        {
            drawVideoLine(p0.y, p0.x, TERM_WIDTH - 1);
            
            for (OEInt i = (p0.y + 1); i < p1.y; i++)
                drawVideoLine(i, 0, TERM_WIDTH - 1);
            
            drawVideoLine(p1.y, 0, p1.x);
        }
        
        imageDidChange = true;
    }
    
    lastSegment = currentSegment;
}

// Copy a 14-pixel segment
#define copySegment(d,s) \
*((OELong *)(d + 0)) = *((OELong *)(s + 0));\
*((OEInt *)(d + 8)) = *((OEInt *)(s + 8));\
*((OEShort *)(d + 12)) = *((OEShort *)(s + 12));

void AppleIIVideo::drawVideoLine(int y, int x0, int x1)
{
    switch (renderer)
    {
        case APPLEII_RENDERER_TEXT:
        {
            OEInt memoryOffset = textOffset[y >> 3];
            OEChar *p = rendererImage + y * ((int) OEWidth(pictureRect)) + x0 * CHAR_WIDTH;
            
            for (OEInt x = x0; x <= x1; x++, p += CHAR_WIDTH)
            {
                OEChar c = rendererTextMemory[memoryOffset + x];
                OEChar *m = (rendererTextMap + (y & 0x7) * MAP_WIDTH +
                              c * MAP_HEIGHT * MAP_WIDTH);
                
                copySegment(p, m);
            }
            
            break;
        }
        case APPLEII_RENDERER_LORES:
        {
            OEInt memoryOffset = textOffset[y >> 3];
            OEChar *p = rendererImage + y * ((int) OEWidth(pictureRect)) + x0 * CHAR_WIDTH;
            
            for (OEInt x = x0; x <= x1; x++, p += CHAR_WIDTH)
            {
                OEChar c = rendererTextMemory[memoryOffset + x];
                OEChar *m = (rendererLoresMap + (y & 0x7) * MAP_WIDTH +
                              c * MAP_HEIGHT * MAP_WIDTH +
                              (x & 1) * MAP_SIZE * MAP_WIDTH * MAP_HEIGHT);
                
                copySegment(p, m);
            }
            
            break;
        }
        case APPLEII_RENDERER_HIRES:
        {
            OEInt memoryOffset = hiresOffset[y];
            OEChar *p = rendererImage + y * ((int) OEWidth(pictureRect)) + x0 * CHAR_WIDTH;
            
            for (OEInt x = x0; x <= x1; x++, p += CHAR_WIDTH)
            {
                // To-Do: Implement Apple IIe delay
                OEInt offset = memoryOffset + x;
                OELong lastOffset = (offset & ~0x7f) | ((offset - 1) & 0x7f);
                
                OEInt c = rendererHiresMemory[offset] | ((rendererHiresMemory[lastOffset] & 0x40) << 2);
                OEChar *m = rendererHiresMap + c * MAP_WIDTH;
                
                copySegment(p, m);
            }
            
            break;
        }
    }
}

void AppleIIVideo::setNeedsDisplay()
{
    pendingSegments = ACTIVE_HEIGHT * TERM_WIDTH;
}

void AppleIIVideo::scheduleNextTimer(OESLong cycles)
{
    updateVideo();
    
    currentTimer++;
    if (currentTimer > APPLEII_TIMER_VSYNC)
        currentTimer = APPLEII_TIMER_ACTIVESTART;
    
    switch (currentTimer)
    {
        case APPLEII_TIMER_ACTIVESTART:
            if (imageDidChange)
            {
                imageDidChange = false;
                
                if (monitor && (powerState != CONTROLBUS_POWERSTATE_OFF))
                    monitor->postMessage(this, CANVAS_POST_IMAGE, &image);
            }
            
            if (powerState == CONTROLBUS_POWERSTATE_ON)
            {
                if (flashCount)
                    flashCount--;
                else
                {
                    flashActive = !flashActive;
                    flashCount = flashFrameNum;
                    
                    updateRendererMap();
                    
                    setNeedsDisplay();
                }
            }
            
            controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &frameStart);
            
            frameStart += cycles;
            
            cycles += OEMinY(activeRect) * 65;
            
            break;
            
        case APPLEII_TIMER_MIXED:
            updateRenderer();
            
            cycles += (OEHeight(activeRect) - 32) * 65;
            
            break;
            
        case APPLEII_TIMER_ACTIVEEND:
            updateRenderer();
            
            cycles += 32 * 65;
            
            break;
            
        case APPLEII_TIMER_VSYNC:
            cycles += (OEMaxY(videoRect) - OEMaxY(activeRect)) * 65;
            
            break;
    }
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
}

AppleIIVideoPoint AppleIIVideo::getCount()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    return count[(size_t) (cycles - frameStart)];
}

OEChar AppleIIVideo::readFloatingBus()
{
    AppleIIVideoPoint count = getCount();
    
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
        // On the Apple II: set A12 on horizontal blanking
        if ((model != APPLEII_MODELIIE) && (count.x < 0x58))
            return vram.hbl[page][address];
        
        return vram.textMain[page][address];
    }
}

void AppleIIVideo::copy(wstring *s)
{
    if (!OEGetBit(mode, MODE_TEXT))
        return;
    
    OEChar charMap[] = {0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x40, 0x60};
    
    OEInt page = OEGetBit(mode, MODE_PAGE2) ? 1 : 0;
    
    OEChar *vp = vram.textMain[page];
    
    if (!vp)
        return;
    
    for (int y = 0; y < TERM_HEIGHT; y++)
    {
        wstring line;
        
        for (int x = 0; x < TERM_WIDTH; x++)
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

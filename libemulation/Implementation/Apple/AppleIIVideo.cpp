
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
#include "AppleIIInterface.h"

#define ACTIVE_HEIGHT       192
#define TERM_WIDTH          40
#define TERM_HEIGHT         24
#define CHAR_WIDTH          14
#define CHAR_HEIGHT         8

#define MAP_SIZE            0x100
#define MAP_SIZE_MASK       0xff
#define MAP_WIDTH           16
#define MAP_HEIGHT          8

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
    model = APPLEIIVIDEO_IIE;
    tvSystem = APPLEIIVIDEO_NTSC;
    characterSet = "Standard";
    flashFrameNum = 16;
    mode = 0;
    
    device = NULL;
    controlBus = NULL;
    floatingBus = NULL;
    ram1 = NULL;
    ram2 = NULL;
    ram3 = NULL;
    videoRAMSync = NULL;
    gamePort = NULL;
    monitorDevice = NULL;
    monitor = NULL;
    
    initPoints();
    initOffsets();
    initLoresMap();
    
    for (int i = 0; i < 2; i++)
    {
        textMemory[i] = NULL;
        textHBLMemory[i] = NULL;
        hiresMemory[i] = NULL;
    }
    
    vector<float> colorBurst;
    colorBurst.push_back(M_PI * -33.0 / 180.0);
    
    image.setSampleRate(14318180);
    image.setFormat(OEIMAGE_LUMINANCE);
    image.setColorBurst(colorBurst);
    imageDidChange = true;
    
    frameStart = 0;
    currentTimer = APPLEIIVIDEO_TIMER_VSYNC;
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
            model = APPLEIIVIDEO_II;
        else if (value == "II j-plus")
            model = APPLEIIVIDEO_IIJPLUS;
        else if (value == "IIe")
            model = APPLEIIVIDEO_IIE;
    }
	else if (name == "revision")
    {
        if (value == "Revision 0")
            revision = APPLEIIVIDEO_REVISION0;
        else if (value == "Revision 1+")
            revision = APPLEIIVIDEO_REVISION1;
        
        isRevisionUpdated = true;
    }
	else if (name == "tvSystem")
    {
        if (value == "NTSC")
            tvSystem = APPLEIIVIDEO_NTSC;
        else if (value == "PAL")
            tvSystem = APPLEIIVIDEO_PAL;
        
        isTVSystemUpdated = true;
    }
	else if (name == "characterSet")
		characterSet = value;
	else if (name == "flashFrameNum")
		flashFrameNum = (OEUInt32) getInt(value);
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
    if (name == "revision")
    {
        switch (revision)
        {
            case APPLEIIVIDEO_REVISION0:
                value = "Revision 0";
                
                break;
                
            case APPLEIIVIDEO_REVISION1:
                value = "Revision 1+";
                
                break;
        }
    }
	else if (name == "tvSystem")
    {
        switch (tvSystem)
        {
            case APPLEIIVIDEO_NTSC:
                value = "NTSC";
                
                break;
                
            case APPLEIIVIDEO_PAL:
                value = "PAL";
                
                break;
        }
    }
	else if (name == "characterSet")
		value = characterSet;
	else if (name == "flashFrameNum")
		value = getString(flashFrameNum);
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
    else if (name == "mmu")
        mmu = ref;
	else if (name == "floatingBus")
		floatingBus = ref;
    else if ((name == "ram1") || (name == "ram"))
        ram1 = ref;
    else if (name == "ram2")
        ram2 = ref;
    else if (name == "ram3")
        ram3 = ref;
    else if (name == "videoRAMSync")
        videoRAMSync = ref;
    else if (name == "gamePort")
    {
        if (gamePort)
            gamePort->removeObserver(this, APPLEIIGAMEPORT_AN2_DID_CHANGE);
        gamePort = ref;
        if (gamePort)
            gamePort->addObserver(this, APPLEIIGAMEPORT_AN2_DID_CHANGE);
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
    
    if (!mmu)
    {
        logMessage("mmu not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    if (!videoRAMSync)
    {
        logMessage("videoRAMSync not connected");
        
        return false;
    }
    
    OEAddress startAddress = 0;
    initVideoRAM(ram1, startAddress);
    initVideoRAM(ram2, startAddress);
    initVideoRAM(ram3, startAddress);
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    initVideoRAMSync();
    
    if (gamePort)
        gamePort->postMessage(this, APPLEIIGAMEPORT_GET_AN2, &an2);
    
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
        if (tvSystem == APPLEIIVIDEO_NTSC)
        {
            videoRect = OEMakeRect(0, 0, 912, 262);
            pictureRect = OEMakeRect(128, 19, 768, 240);
            activeRect = OEMakeRect(232, 38, 560, 192);
        }
        else if (tvSystem == APPLEIIVIDEO_PAL)
        {
            videoRect = OEMakeRect(0, 0, 912, 312);
            pictureRect = OEMakeRect(128, 21, 768, 288);
            activeRect = OEMakeRect(232, 48, 560, 192);
        }
        
        updateSegments();
        updateCounts();
        updateImage();
        updateClockFrequency();
        
        currentTimer = APPLEIIVIDEO_TIMER_VSYNC;
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
        case APPLEIIVIDEO_REFRESH:
            setNeedsDisplay();
            
            return true;
            
        case APPLEIIVIDEO_READ_FLOATINGBUS:
            *((OEUInt8 *)data) = readFloatingBus();
            
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
                scheduleNextTimer(*((OEInt64 *)data));
                
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
            setMode(APPLEIIVIDEO_TEXT, address & 0x1);
            
            break;
            
        case 0x52: case 0x53:
            setMode(APPLEIIVIDEO_MIXED, address & 0x1);
            
            break;
            
        case 0x54: case 0x55:
            setMode(APPLEIIVIDEO_PAGE2, address & 0x1);
            
            break;
            
        case 0x56: case 0x57:
            setMode(APPLEIIVIDEO_HIRES, address & 0x1);
            
            break;
    }
}

void AppleIIVideo::updateSegments()
{
    int cycleNum = 65 * OEHeight(videoRect) + 256;
    
    segment.resize(cycleNum);
    
    for (int i = 0; i < cycleNum; i++)
    {
        int xv = i % 65 - (65 - TERM_WIDTH);
        int yv = i / 65 - OEMinY(activeRect);
        
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
    
    for (int i = 0; i < TERM_WIDTH * ACTIVE_HEIGHT; i++)
    {
        point[i].x = i % TERM_WIDTH;
        point[i].y = i / TERM_WIDTH;
    }
}

void AppleIIVideo::updateCounts()
{
    int cycleNum = 65 * OEHeight(videoRect) + 16;
    
    count.resize(cycleNum);
    
    int videoHeight = (int) OEHeight(videoRect);
    
    for (int i = 0; i < cycleNum; i++)
    {
        int x = i % 65;
        int y = i / 65;
        
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
    
    for (int y = 0; y < TERM_HEIGHT; y++)
        textOffset[y] = (y >> 3) * TERM_WIDTH + (y & 0x7) * 0x80;
    
    hiresOffset.resize(TERM_HEIGHT * CHAR_HEIGHT);
    
    for (int y = 0; y < TERM_HEIGHT * CHAR_HEIGHT; y++)
        hiresOffset[y] = (y >> 6) * TERM_WIDTH + ((y >> 3) & 0x7) * 0x80 + (y & 0x7) * 0x400;
}

void AppleIIVideo::loadTextMap(string name, OEData *data)
{
    if (data->size() < MAP_HEIGHT)
        return;
    
    OEData theMap;
    
    theMap.resize(4 * MAP_SIZE * MAP_HEIGHT * MAP_WIDTH);
    
    int cMask = (int) getNextPowerOf2(data->size() / MAP_HEIGHT) - 1;
    
    for (int c = 0; c < 4 * MAP_SIZE; c++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            int cr = c;
            
            if (model == APPLEIIVIDEO_IIJPLUS)
            {
                OESetBit(cr, 0x40, OEGetBit(c, 0x80));
                OESetBit(cr, 0x80, OEGetBit(c, 0x200));
            }
            
            OEUInt8 byte = data->at((cr & cMask) * MAP_HEIGHT + y);
            
            OESetBit(byte, 0x80, (c & 0xc0) == 0x40);
            
            bool inv = !(OEGetBit(c, 0x80) || (OEGetBit(byte, 0x80) && OEGetBit(c, 0x100)));
            
            for (int x = 0; x < MAP_WIDTH; x++)
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
    
    for (int c = 0; c < 2 * MAP_SIZE; c++)
    {
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            OEUInt32 v = (y < 4) ? c & 0xf : (c >> 4) & 0xf;
            
            OEUInt32 bitmap = (v << 12) | (v << 8) | (v << 4) | v;
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
    
    for (int c = 0; c < 2 * MAP_SIZE; c++)
    {
        OEUInt8 byte = (c & 0x7f) << 1 | (c >> 8);
        bool delay = (revision != APPLEIIVIDEO_REVISION0) && (c & 0x80);
        
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            bool b = (byte >> ((x + 2 - delay) >> 1)) & 0x1;
            
            hiresMap[c * MAP_WIDTH + x] = b ? 0xff : 0x00;
        }
    }
}

void AppleIIVideo::initVideoRAM(OEComponent *ram, OEAddress& startAddress)
{
    // Notes:
    // * Requires memory to be mapped contiguously
    // * For hires to work, memory at $2000 and $4000 should be in a single block
    
    if (!ram)
        return;
    
    OEData *data;
    ram->postMessage(this, RAM_GET_DATA, &data);
    
    OEAddress endAddress = startAddress + data->size() - 1;
    
    if ((startAddress <= 0x400) && (endAddress >= 0x7ff))
        textMemory[0] = &data->front() + 0x400 - startAddress;
    if ((startAddress <= 0x800) && (endAddress >= 0xbff))
        textMemory[1] = &data->front() + 0x800 - startAddress;
    if ((startAddress <= 0x1400) && (endAddress >= 0x17ff))
        textHBLMemory[0] = &data->front() + 0x1400 - startAddress;
    if ((startAddress <= 0x1800) && (endAddress >= 0x1bff))
        textHBLMemory[1] = &data->front() + 0x1800 - startAddress;
    if ((startAddress <= 0x2000) && (endAddress >= 0x3fff))
        hiresMemory[0] = &data->front() + 0x2000 - startAddress;
    if ((startAddress <= 0x4000) && (endAddress >= 0x5fff))
        hiresMemory[1] = &data->front() + 0x4000 - startAddress;
    
    startAddress += data->size();
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
    
    if (tvSystem == APPLEIIVIDEO_NTSC)
        clockFrequency = 14318180.0 * 65 / 912;
    else if (tvSystem == APPLEIIVIDEO_PAL)
        clockFrequency = 14250450.0 * 65 / 912;
    
    controlBus->postMessage(this, CONTROLBUS_SET_CLOCKFREQUENCY, &clockFrequency);
}

void AppleIIVideo::updateRendererMap()
{
    rendererTextMap = (OEUInt8 *)&textMap[characterSet].front();
    
    if (an2)
        rendererTextMap += 2 * MAP_SIZE * MAP_WIDTH * MAP_HEIGHT;
    if (flashActive)
        rendererTextMap += MAP_SIZE * MAP_WIDTH * MAP_HEIGHT;
    
    rendererLoresMap = (OEUInt8 *)&loresMap.front();
    
    rendererHiresMap = (OEUInt8 *)&hiresMap.front();
}

void AppleIIVideo::updateRenderer()
{
    if ((revision == APPLEIIVIDEO_REVISION0) || !OEGetBit(mode, APPLEIIVIDEO_TEXT))
        image.setSubcarrier(3579545);
    else
        image.setSubcarrier(0);
    
    OEUInt32 page = OEGetBit(mode, APPLEIIVIDEO_PAGE2) ? 1 : 0;
    
    rendererTextMemory = textMemory[page];
    rendererHiresMemory = hiresMemory[page];
    
    switch (currentTimer)
    {
        case APPLEIIVIDEO_TIMER_MIXED:
            if (OEGetBit(mode, APPLEIIVIDEO_TEXT))
                renderer = APPLEIIVIDEO_RENDERER_TEXT;
            else if (!OEGetBit(mode, APPLEIIVIDEO_HIRES))
                renderer = APPLEIIVIDEO_RENDERER_LORES;
            else
                renderer = APPLEIIVIDEO_RENDERER_HIRES;
            
            break;
            
        case APPLEIIVIDEO_TIMER_ACTIVEEND:
            if (OEGetBit(mode, APPLEIIVIDEO_TEXT) ||
                OEGetBit(mode, APPLEIIVIDEO_MIXED))
                renderer = APPLEIIVIDEO_RENDERER_TEXT;
            else if (!OEGetBit(mode, APPLEIIVIDEO_HIRES))
                renderer = APPLEIIVIDEO_RENDERER_LORES;
            else
                renderer = APPLEIIVIDEO_RENDERER_HIRES;
            
            break;
            
        default:
            break;
    }
}

void AppleIIVideo::setMode(OEUInt32 mask, bool value)
{
    OEUInt32 oldMode = mode;
    
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
    OEUInt64 cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    int currentSegment = segment[(size_t) (cycles - frameStart)];
    
    int deltaSegment = currentSegment - lastSegment;
    
    if (deltaSegment < 0)
        deltaSegment += ACTIVE_HEIGHT * TERM_WIDTH;
    
    int segmentNum = min(deltaSegment, pendingSegments);
    
    if (segmentNum)
    {
        pendingSegments -= segmentNum;
        
        int thisSegment = lastSegment + segmentNum;
        
        AppleIIVideoPoint p0 = point[lastSegment];
        AppleIIVideoPoint p1 = point[thisSegment - 1];
        
        if (p0.y == p1.y)
            drawVideoLine(p0.y, p0.x, p1.x);
        else
        {
            drawVideoLine(p0.y, p0.x, TERM_WIDTH - 1);
            
            for (OEUInt32 i = (p0.y + 1); i < p1.y; i++)
                drawVideoLine(i, 0, TERM_WIDTH - 1);
            
            drawVideoLine(p1.y, 0, p1.x);
        }
        
        imageDidChange = true;
    }
    
    lastSegment = currentSegment;
}

// Copy a 14-pixel segment
#define copySegment(d,s) \
*((OEUInt64 *)(d + 0)) = *((OEUInt64 *)(s + 0));\
*((OEUInt32 *)(d + 8)) = *((OEUInt32 *)(s + 8));\
*((OEUInt16 *)(d + 12)) = *((OEUInt16 *)(s + 12));

void AppleIIVideo::drawVideoLine(int y, int x0, int x1)
{
    switch (renderer)
    {
        case APPLEIIVIDEO_RENDERER_TEXT:
        {
            OEUInt64 memoryOffset = textOffset[y >> 3];
            OEUInt8 *p = rendererImage + y * ((int) OEWidth(pictureRect)) + x0 * CHAR_WIDTH;
            
            for (OEUInt64 x = x0; x <= x1; x++, p += CHAR_WIDTH)
            {
                OEUInt8 c = rendererTextMemory[memoryOffset + x];
                OEUInt8 *m = (rendererTextMap + (y & 0x7) * MAP_WIDTH +
                              c * MAP_HEIGHT * MAP_WIDTH);
                
                copySegment(p, m);
            }
            
            break;
        }
        case APPLEIIVIDEO_RENDERER_LORES:
        {
            OEUInt64 memoryOffset = textOffset[y >> 3];
            OEUInt8 *p = rendererImage + y * ((int) OEWidth(pictureRect)) + x0 * CHAR_WIDTH;
            
            for (OEUInt64 x = x0; x <= x1; x++, p += CHAR_WIDTH)
            {
                OEUInt8 c = rendererTextMemory[memoryOffset + x];
                OEUInt8 *m = (rendererLoresMap + (y & 0x7) * MAP_WIDTH +
                              c * MAP_HEIGHT * MAP_WIDTH +
                              (x & 1) * MAP_SIZE * MAP_WIDTH * MAP_HEIGHT);
                
                copySegment(p, m);
            }
            
            break;
        }
        case APPLEIIVIDEO_RENDERER_HIRES:
        {
            OEUInt64 memoryOffset = hiresOffset[y];
            OEUInt8 *p = rendererImage + y * ((int) OEWidth(pictureRect)) + x0 * CHAR_WIDTH;
            
            for (OEUInt64 x = x0; x <= x1; x++, p += CHAR_WIDTH)
            {
                // To-Do: Implement Apple IIe delay
                OEUInt64 offset = memoryOffset + x;
                OEUInt64 lastOffset = (offset & ~0x7f) | ((offset - 1) & 0x7f);
                
                OEUInt32 c = rendererHiresMemory[offset] | ((rendererHiresMemory[lastOffset] & 0x40) << 2);
                OEUInt8 *m = rendererHiresMap + c * MAP_WIDTH;
                
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

void AppleIIVideo::initVideoRAMSync()
{
    MemoryMap ramSyncMap;
    
    ramSyncMap.component = videoRAMSync;
    ramSyncMap.startAddress = 0x400;
    ramSyncMap.endAddress = 0xbff;
    ramSyncMap.read = false;
    ramSyncMap.write = true;
    mmu->postMessage(this, MMU_MAP_MEMORY, &ramSyncMap);
    
    ramSyncMap.component = videoRAMSync;
    ramSyncMap.startAddress = 0x2000;
    ramSyncMap.endAddress = 0x5fff;
    ramSyncMap.read = false;
    ramSyncMap.write = true;
    mmu->postMessage(this, MMU_MAP_MEMORY, &ramSyncMap);
}

void AppleIIVideo::scheduleNextTimer(OEInt64 cycles)
{
    updateVideo();
    
    currentTimer++;
    if (currentTimer > APPLEIIVIDEO_TIMER_VSYNC)
        currentTimer = APPLEIIVIDEO_TIMER_ACTIVESTART;
    
    switch (currentTimer)
    {
        case APPLEIIVIDEO_TIMER_ACTIVESTART:
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
            
        case APPLEIIVIDEO_TIMER_MIXED:
            updateRenderer();
            
            cycles += (OEHeight(activeRect) - 32) * 65;
            
            break;
            
        case APPLEIIVIDEO_TIMER_ACTIVEEND:
            updateRenderer();
            
            cycles += 32 * 65;
            
            break;
            
        case APPLEIIVIDEO_TIMER_VSYNC:
            cycles += (OEMaxY(videoRect) - OEMaxY(activeRect)) * 65;
            
            break;
    }
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
}

AppleIIVideoPoint AppleIIVideo::getCount()
{
    OEUInt64 cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    return count[(size_t) (cycles - frameStart)];
}

OEUInt8 AppleIIVideo::readFloatingBus()
{
    AppleIIVideoPoint count = getCount();
    
	bool mixed = OEGetBit(mode, APPLEIIVIDEO_MIXED) && ((count.y & 0xa0) == 0xa0);
	bool hires = OEGetBit(mode, APPLEIIVIDEO_HIRES) && !(OEGetBit(mode, APPLEIIVIDEO_TEXT) || mixed);
	bool page = OEGetBit(mode, APPLEIIVIDEO_PAGE2);
	
	OEUInt32 address = count.x & 0x7;
	
    OEUInt32 h5h4h3 = count.x & 0x38;
    
    OEUInt32 v4v3 = count.y & 0xc0;
	OEUInt32 v4v3v4v3 = (v4v3 >> 3) | (v4v3 >> 1);
	
    address |= (0x68 + h5h4h3 + v4v3v4v3) & 0x78;
	address |= (count.y & 0x38) << 4;
    
	if (hires)
    {
		address |= (count.y & 0x7) << 10;
        
        return hiresMemory[page][address];
	}
    else
    {
        // On the Apple II: set A12 on horizontal blanking
        if ((revision != APPLEIIVIDEO_IIE) && (count.x < 0x58))
            return textHBLMemory[page][address];
        
        return textMemory[page][address];
    }
}

void AppleIIVideo::copy(wstring *s)
{
    if (!OEGetBit(mode, APPLEIIVIDEO_TEXT))
        return;
    
    OEUInt8 charMap[] = {0x40, 0x20, 0x40, 0x20, 0x40, 0x20, 0x40, 0x60};
    
    OEUInt32 page = OEGetBit(mode, APPLEIIVIDEO_PAGE2) ? 1 : 0;
    
    OEUInt8 *vp = textMemory[page];
    
    if (!vp)
        return;
    
    for (int y = 0; y < TERM_HEIGHT; y++)
    {
        wstring line;
        
        for (int x = 0; x < TERM_WIDTH; x++)
        {
            OEUInt8 value = vp[textOffset[y] + x];
            
            // To-Do: Mousetext
            line += charMap[value >> 5] | (value & 0x1f);
        }
        
        line = rtrim(line);
        line += '\n';
        
        *s += line;
    }
}

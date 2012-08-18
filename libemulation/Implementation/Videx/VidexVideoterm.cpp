
/**
 * libemulation
 * Videx Videoterm
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a Videx Videoterm
 */

#include <math.h>

#include "VidexVideoterm.h"

#include "MemoryInterface.h"
#include "AppleIIInterface.h"

#define CLOCK_FREQUENCY 17430000

#define RAM_SIZE        0x800
#define RAM_MASK        (RAM_SIZE - 1)

#define FONT_CHARNUM    0x80
#define FONT_CHARWIDTH  8
#define FONT_CHARHEIGHT 16
#define FONT_CHARSIZE   (FONT_CHARWIDTH * FONT_CHARHEIGHT)
#define FONT_SIZE       (FONT_CHARNUM * FONT_CHARSIZE)

enum
{
    OUTPUT_AUTO,
    OUTPUT_ON,
    OUTPUT_OFF,
};

VidexVideoterm::VidexVideoterm() : MC6845()
{
    ram = NULL;
    bankSwitcher = NULL;
    video = NULL;
    gamePort = NULL;
    
    ramBank = 0;
    
    an0 = false;
    colorKiller = false;
    
    monitorConnected = false;
    monitorCaptured = false;
    cellWidth = 9;
    
    image.setFormat(OEIMAGE_LUMINANCE);
    
    videoOutput = OUTPUT_AUTO;
}

bool VidexVideoterm::setValue(string name, string value)
{
    if (name == "characterSet1")
        characterSet1 = value;
    else if (name == "characterSet2")
        characterSet2 = value;
    else if (name == "ramBank")
        ramBank = getOEInt(value);
    else if (name == "cellWidth")
        cellWidth = getOEInt(value);
    else if (name == "videoOutput")
    {
        if (value == "On")
            videoOutput = OUTPUT_ON;
        else if (value == "Off")
            videoOutput = OUTPUT_OFF;
        else
            videoOutput = OUTPUT_AUTO;
    }
    else
        return MC6845::setValue(name, value);
    
    return true;
}

bool VidexVideoterm::getValue(string name, string &value)
{
    if (name == "characterSet1")
        value = characterSet1;
    else if (name == "characterSet2")
        value = characterSet2;
    else if (name == "ramBank")
        value = getString(ramBank);
    else if (name == "cellWidth")
        value = getString(cellWidth);
    else if (name == "videoOutput")
    {
        if (videoOutput == OUTPUT_ON)
            value = "On";
        else if (videoOutput == OUTPUT_OFF)
            value = "Off";
        else
            value = "Auto";
    }
    else
        return MC6845::getValue(name, value);
    
    return true;
}

bool VidexVideoterm::setRef(string name, OEComponent *ref)
{
    if (name == "ram")
        ram = ref;
    else if (name == "bankSwitcher")
        bankSwitcher = ref;
    else if (name == "video")
    {
        if (video)
        {
            video->removeObserver(this, CANVAS_DID_COPY);
            video->removeObserver(this, APPLEII_MONITOR_DID_CHANGE);
            video->removeObserver(this, APPLEII_COLORKILLER_DID_CHANGE);
        }
        video = ref;
        if (video)
        {
            video->addObserver(this, CANVAS_DID_COPY);
            video->addObserver(this, APPLEII_MONITOR_DID_CHANGE);
            video->addObserver(this, APPLEII_COLORKILLER_DID_CHANGE);
        }
    }
    else if (name == "gamePort")
    {
        if (gamePort)
            gamePort->removeObserver(this, APPLEII_AN0_DID_CHANGE);
        gamePort = ref;
        if (gamePort)
            gamePort->addObserver(this, APPLEII_AN0_DID_CHANGE);
    }
    else
        return MC6845::setRef(name, ref);
    
    return true;
}

bool VidexVideoterm::setData(string name, OEData *data)
{
    if (name.substr(0, 4) == "font")
        return loadFont(name.substr(4), data);
    else
        return MC6845::setData(name, data);
    
    return true;
}

bool VidexVideoterm::init()
{
    OECheckComponent(ram);
    OECheckComponent(bankSwitcher);
    OECheckComponent(video);
    
    if (!MC6845::init())
        return false;
    
    ram->postMessage(this, RAM_GET_DATA, &vram);
    
    if (vram->size() < RAM_SIZE)
    {
        logMessage("ram not large enough");
        
        return false;
    }
    
    drawMemory = &vram->front();
    
    updateRAMBank();
    
    if (video)
    {
        video->postMessage(this, APPLEII_IS_MONITOR_CONNECTED, &monitorConnected);
        video->postMessage(this, APPLEII_IS_COLORKILLER_ENABLED, &colorKiller);
    }
    if (gamePort)
        gamePort->postMessage(this, APPLEII_GET_AN0, &an0);
    
    currentFont.resize(4 * FONT_SIZE);
    
    draw = (MC6845Draw) &VidexVideoterm::drawLine;
    drawFont = &currentFont.front();
    
    updateVideoEnabled();
    
    update();
    
    return true;
}

void VidexVideoterm::update()
{
    if (font.count(characterSet1))
        memcpy(&currentFont.front(),
               &font[characterSet1].front(), FONT_SIZE);
    else
        memset(&currentFont.front(), 0, FONT_SIZE);
    
    if (font.count(characterSet2))
        memcpy(&currentFont.front() + FONT_SIZE,
               &font[characterSet2].front(), FONT_SIZE);
    else
        memset(&currentFont.front() + FONT_SIZE, 0, FONT_SIZE);
    
    for (OEInt i = 0; i < 2 * FONT_SIZE; i++)
        currentFont[i + 2 * FONT_SIZE] = ~currentFont[i];
    
    updateVideoEnabled();
    
    refreshVideo();
}

void VidexVideoterm::dispose()
{
    monitorConnected = false;
    
    updateVideoEnabled();
    
    MC6845::dispose();
}

void VidexVideoterm::notify(OEComponent *sender, int notification, void *data)
{
    MC6845::notify(sender, notification, data);
    
    if (sender == ram)
        refreshVideo();
    else if (sender == video)
    {
        switch (notification)
        {
            case CANVAS_DID_COPY:
                copy((wstring *)data);
                
                break;
                
            case APPLEII_MONITOR_DID_CHANGE:
                monitorConnected = *((bool *) data);
                
                updateVideoEnabled();
                
                break;
                
            case APPLEII_COLORKILLER_DID_CHANGE:
                colorKiller = *((bool *)data);
                
                updateVideoEnabled();
                
                break;
        }
    }
    else if (sender == gamePort)
    {
        an0 = *((bool *)data);
        
        updateVideoEnabled();
    }
}

OEChar VidexVideoterm::read(OEAddress address)
{
    setCellWidth(((address >> 1) & 0x1) ? 8 : 9);
    setRAMBank((OEInt) (address >> 2) & 0x3);
    
    return MC6845::read(address);
}

void VidexVideoterm::write(OEAddress address, OEChar value)
{
    setCellWidth(((address >> 1) & 0x1) ? 8 : 9);
    setRAMBank((OEInt) (address >> 2) & 0x3);
    
    MC6845::write(address, value);
}

bool VidexVideoterm::loadFont(string name, OEData *data)
{
    if (data->size() < (FONT_CHARNUM * FONT_CHARHEIGHT))
        return false;
    
    OEData theFont;
    theFont.resize(FONT_SIZE);
    
    for (OEInt i = 0; i < FONT_CHARNUM; i++)
    {
        for (OEInt y = 0; y < FONT_CHARHEIGHT; y++)
        {
            OEChar value = (*data)[i * FONT_CHARHEIGHT + y];
            
            for (OEInt x = 0; x < FONT_CHARWIDTH; x++)
            {
                OEChar pixel = ((value & 0x80) ? 0xff : 0x00);
                
                theFont[(i * FONT_CHARHEIGHT + y) * FONT_CHARWIDTH + x] = pixel;
                
                value <<= 1;
            }
        }
    }
    
    font[name] = theFont;
    
    return true;
}

void VidexVideoterm::setRAMBank(OEInt value)
{
    if (ramBank != value)
    {
        ramBank = value;
        
        updateRAMBank();
    }
}

void VidexVideoterm::updateRAMBank()
{
    AddressOffsetMap offsetMap;
    
    offsetMap.startAddress = 0x000;
    offsetMap.endAddress = 0x1ff;
    offsetMap.offset = 0x200 * ramBank - 0x400;
    
    bankSwitcher->postMessage(this, ADDRESSOFFSET_MAP, &offsetMap);
}

void VidexVideoterm::setCellWidth(OEInt value)
{
    if (cellWidth != value)
    {
        cellWidth = value;
        
        updateTiming();
    }
}

void VidexVideoterm::updateVideoEnabled()
{
    bool videoOutputEnabled;
    
    if (videoOutput == OUTPUT_AUTO)
        videoOutputEnabled = (an0 && colorKiller);
    else
        videoOutputEnabled = (videoOutput == OUTPUT_ON);
    
    bool newMonitorCaptured = (videoOutputEnabled &&
                               monitorConnected &&
                               (powerState != CONTROLBUS_POWERSTATE_OFF));
    
    bool newVideoEnabled = (newMonitorCaptured &&
                            !inReset);
    
    if (videoEnabled != newVideoEnabled)
    {
        videoEnabled = newVideoEnabled;
        
        if (!videoEnabled)
        {
            image.fill(OEColor());
            
            video->postMessage(this, CANVAS_CLEAR, NULL);
        }
        else
            refreshVideo();
    }
    
    if (monitorCaptured != newMonitorCaptured)
    {
        monitorCaptured = newMonitorCaptured;
        
        video->postMessage(this, (monitorCaptured ? APPLEII_REQUEST_MONITOR :
                                  APPLEII_RELEASE_MONITOR), NULL);
    }
}

void VidexVideoterm::updateTiming()
{
    // Update MC6845
    clockFrequency = CLOCK_FREQUENCY / cellWidth;
    
    MC6845::updateTiming();
    
    // Update parameters
    OESInt horizStart = horizTotal - horizSyncPosition;
    OESInt vertStart = vertTotal - vertSyncPosition;
    
    OERect displayRect = OEMakeRect(horizStart, vertStart,
                                    horizDisplayed, vertDisplayed);
    displayRect = OEIntersectionRect(displayRect,
                                     OEMakeRect(0, 0, horizTotal, vertTotal));
    
    OERect visibleRect = OEMakeRect(clockFrequency * NTSC_HSTART, NTSC_VSTART,
                                    clockFrequency * NTSC_HLENGTH, NTSC_VLENGTH);
    
    displayRect = OEIntersectionRect(displayRect, visibleRect);
    displayRect = OEIntegralRect(displayRect);
    
    // Update image
    image.setSize(OEIntegralSize(OEMakeSize(cellWidth * visibleRect.size.width,
                                            visibleRect.size.height)));
    imageWidth = image.getSize().width;
    
    imagep = image.getPixels();
    imagep += ((OEInt) ((vertStart - OEMinY(visibleRect)) * imageWidth) +
               (OEInt) ((horizStart - OEMinX(visibleRect)) * cellWidth));
    image.setSampleRate(CLOCK_FREQUENCY);
    
    // Update pos data
    OEInt cycleNum = frameCycleNum + 16;
    
    pos.resize(cycleNum);
    
    for (OEInt i = 0; i < cycleNum; i++)
    {
        OEPoint p = OEGetPosInRect(OEMakePoint(i % horizTotal, i / horizTotal), displayRect);
        
        pos[i].x = p.x - horizStart;
        pos[i].y = p.y - vertStart;
    }
    
    posXBegin = OEMinX(displayRect) - horizStart;
    posXEnd = OEMaxX(displayRect) - horizStart;
}

// Copy an 8-pixel segment
#define copySegment(d,s) \
*((OELong *)(d + 0)) = *((OELong *)(s + 0));\
*((OEChar *)(d + 8)) = *((OEChar *)(s + 7))

void VidexVideoterm::drawLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = (frameStartAddress.d.l + (y / scanline) * horizDisplayed);
    OEChar *p = imagep + y * imageWidth + x0 * cellWidth;
    
    for (OEInt x = x0; x < x1; x++, p += cellWidth)
    {
        OEInt address = (memoryOffset + x);
        OEInt i = drawMemory[address & RAM_MASK];
        OEChar *m;
        
        if ((address == cursorAddress.w.l) && blink)
        {
            OEInt rasterY = y % scanline;
            
            if ((rasterY >= (cursorStart & 0x1f)) &&
                (rasterY <= cursorEnd))
                i += 2 * FONT_CHARNUM;
        }
        
        m = drawFont + ((y % scanline) & 0xf) * FONT_CHARWIDTH + i * FONT_CHARSIZE;
        
        copySegment(p, m);
    }
}

void VidexVideoterm::postImage()
{
    video->postMessage(this, CANVAS_POST_IMAGE, &image);
}

void VidexVideoterm::copy(wstring *s)
{
    if (!videoEnabled)
        return;
    
    OEChar *vp = &vram->front();
    
    for (OEInt y = 0; y < vertDisplayedCell; y++)
    {
        wstring line;
        
        for (OEInt x = 0; x < horizDisplayed; x++)
            line += vp[(frameStartAddress.w.l + y * horizDisplayed + x) & RAM_MASK];
        
        line = rtrim(line);
        line += '\n';
        
        *s += line;
    }
}

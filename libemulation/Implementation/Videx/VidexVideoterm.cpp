
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

#define FONT_CHARNUM    0x80
#define FONT_CHARWIDTH  8
#define FONT_CHARHEIGHT 16
#define FONT_CHARSIZE   (FONT_CHARWIDTH * FONT_CHARHEIGHT)
#define FONT_SIZE       (FONT_CHARNUM * FONT_CHARSIZE)

VidexVideoterm::VidexVideoterm() : MC6845()
{
    video = NULL;
    ram = NULL;
    gamePort = NULL;
    
    ramBank = 0;
    
    an0 = false;
    colorKiller = false;
    
    monitorRequest = false;
    cellWidth = 0;
    
    
    image.setFormat(OEIMAGE_LUMINANCE);
}

bool VidexVideoterm::setValue(string name, string value)
{
    if (name == "cellWidth")
        cellWidth = getOEInt(value);
    else if (name == "ramBank")
        ramBank = getOEInt(value);
    else if (name == "characterSet1")
        characterSet1 = value;
    else if (name == "characterSet2")
        characterSet2 = value;
    else
        return MC6845::setValue(name, value);
    
    return true;
}

bool VidexVideoterm::getValue(string name, string &value)
{
    if (name == "cellWidth")
        value = getString(cellWidth);
    else if (name == "ramBank")
        value = getString(ramBank);
    else if (name == "characterSet1")
        value = characterSet1;
    else if (name == "characterSet2")
        value = characterSet2;
    else
        return MC6845::getValue(name, value);
    
    return true;
}

bool VidexVideoterm::setRef(string name, OEComponent *ref)
{
    if (name == "video")
    {
        if (video)
        {
            video->removeObserver(this, CANVAS_DID_COPY);
            video->removeObserver(this, APPLEII_COLORKILLER_DID_CHANGE);
        }
        video = ref;
        if (video)
        {
            video->addObserver(this, CANVAS_DID_COPY);
            video->addObserver(this, APPLEII_COLORKILLER_DID_CHANGE);
        }
    }
    else if (name == "ram")
        ram = ref;
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
    if (!MC6845::init())
        return false;
    
    if (!video)
    {
        logMessage("video not connected");
        
        return false;
    }
    
    if (!ram)
    {
        logMessage("ram not connected");
        
        return false;
    }
    
    updateRAMBank();
    
    if (video)
        video->postMessage(this, APPLEII_GET_COLORKILLER, &colorKiller);
    if (gamePort)
        gamePort->postMessage(this, APPLEII_GET_AN0, &an0);
    
    ram->postMessage(this, RAM_GET_DATA, &vram);
    drawMemory = &vram->front();
    
    currentFont.resize(2 * FONT_SIZE);
    drawFont = &currentFont.front();
    draw = (MC6845Draw) &VidexVideoterm::drawLine;
    
    updateMonitorRequest();
    
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
}

void VidexVideoterm::dispose()
{
    an0 = false;
    colorKiller = false;
    
    updateMonitorRequest();
}

void VidexVideoterm::notify(OEComponent *sender, int notification, void *data)
{
    MC6845::notify(sender, notification, data);
    
    if (sender == video)
    {
        switch (notification)
        {
            case CANVAS_DID_COPY:
                return copy((wstring *)data);
                
            case APPLEII_COLORKILLER_DID_CHANGE:
                colorKiller = *((bool *)data);
                
                break;
        }
    }
    else if (sender == gamePort)
        an0 = *((bool *)data);
    
    updateMonitorRequest();
}

OEChar VidexVideoterm::read(OEAddress address)
{
    setCellWidth(((address >> 1) & 0x1) ? 8 : 9);
    setRAMBank((address >> 2) & 0x3);
    
    return MC6845::read(address);
}

void VidexVideoterm::write(OEAddress address, OEChar value)
{
    setCellWidth(((address >> 1) & 0x1) ? 8 : 9);
    setRAMBank((address >> 2) & 0x3);
    
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
                theFont[(i * FONT_CHARHEIGHT + y) * FONT_CHARWIDTH + x] = ((value & 0x80) ?
                                                                           0xff : 0x00);
                
                value <<= 1;
            }
        }
    }
    
    font[name] = theFont;
    
    return true;
}

void VidexVideoterm::setRAMBank(OEInt value)
{
    if (ramBank == value)
        return;
    
    ramBank = value;
    
    updateRAMBank();
}

void VidexVideoterm::updateRAMBank()
{
    BankSwitchedRAMMap ramMap;
    
    ramMap.startAddress = 0x400;
    ramMap.endAddress = 0x5ff;
    ramMap.offset = 0x200 * ramBank - 0x400;
    
    ram->postMessage(this, BANKSWITCHEDRAM_MAP, &ramMap);
}

void VidexVideoterm::setCellWidth(OEInt value)
{
    if (cellWidth == value)
        return;
    
    cellWidth = value;
    
    updateTiming();
}

void VidexVideoterm::updateMonitorRequest()
{
    bool newMonitorRequest = an0 && colorKiller;
    
    if (monitorRequest != newMonitorRequest)
    {
        monitorRequest = newMonitorRequest;
        
        video->postMessage(this, (monitorRequest ? APPLEII_REQUEST_MONITOR :
                                  APPLEII_RELEASE_MONITOR), NULL);
    }
}

void VidexVideoterm::updateTiming()
{
    clockFrequency = CLOCK_FREQUENCY / cellWidth;
    
    MC6845::updateTiming();
    
    // Update parameters
    OERect visibleRect;
    OERect displayRect;
    
    vertTotal = NTSC_VTOTAL;
    
    OESInt horizStart = horizTotal - horizSyncPosition;
    OESInt vertStart = vertTotal - vertSyncPosition;
    
    visibleRect = OEMakeRect((OEInt) (clockFrequency * cellWidth * NTSC_HSTART), NTSC_VSTART,
                             (OEInt) (clockFrequency * cellWidth * NTSC_HLENGTH), NTSC_VLENGTH);
    displayRect = OEMakeRect(cellWidth * horizStart, vertStart,
                             cellWidth * horizDisplayed, vertDisplayed);
    
    // Resize image
    image.setSize(visibleRect.size);
    imageWidth = image.getSize().width;
    
    imagep = image.getPixels();
//    imagep += (OEInt) ((OEMinY(displayRect) - OEMinY(visibleRect)) * imageWidth);
    imagep += (OEInt) (OEMinX(displayRect) - OEMinX(visibleRect));
    image.setSampleRate(CLOCK_FREQUENCY);
    
    // Build pos data
    OEInt cycleNum = frameCycleNum + 16;
    
    OESInt visibleStart = ceil((OEMinX(visibleRect) - OEMinX(displayRect)) / cellWidth);
    OESInt visibleEnd = ceil((OEMaxX(visibleRect) - OEMinX(displayRect)) / cellWidth);
    
    posBegin = 0;
    posEnd = horizDisplayed;
    
    if (posBegin < visibleStart)
        posBegin = visibleStart;
    else if (posBegin > visibleEnd)
        posBegin = visibleEnd;
    
    if (posEnd < visibleStart)
        posEnd = visibleStart;
    else if (posEnd > visibleEnd)
        posEnd = visibleEnd;
    
    pos.resize(cycleNum);
    
    for (OEInt i = 0; i < cycleNum; i++)
    {
        OESInt sx = i % 65;
        OESInt sy = i / 65;
        
        pos[i].x = sx - horizStart;
        pos[i].y = sy - vertStart;
        
        if (pos[i].x < posBegin)
            pos[i].x = posBegin;
        if (pos[i].x > posEnd)
            pos[i].x = posEnd;
        
        if (pos[i].y < 0)
        {
            pos[i].x = 0;
            pos[i].y = 0;
        }
        if (pos[i].y >= vertDisplayed)
        {
            pos[i].x = posEnd;
            pos[i].y = vertDisplayed - 1;
        }
    }
}

// Copy an 8-pixel segment
#define copySegment(d,s) \
*((OELong *)(d + 0)) = *((OELong *)(s + 0));\
*((OEChar *)(d + 8)) = *((OEChar *)(s + 7))

void VidexVideoterm::drawLine(OESInt y, OESInt x0, OESInt x1)
{
    OEInt memoryOffset = (startAddress.d.l + (y / scanline) * horizDisplayed) & 0x7ff;
    OEChar *p = imagep + y * imageWidth + x0 * cellWidth;
    
    for (OEInt x = x0; x < x1; x++, p += cellWidth)
    {
        OEChar i = drawMemory[memoryOffset + x];
        OEChar *m = drawFont + (y % scanline) * FONT_CHARWIDTH + i * FONT_CHARSIZE;
        
        copySegment(p, m);
    }
}

void VidexVideoterm::postImage()
{
    video->postMessage(this, CANVAS_POST_IMAGE, &image);
}

void VidexVideoterm::copy(wstring *s)
{
    if (vram->size() < (vertDisplayedCell * horizDisplayed))
        return;
    
    OEChar *vp = &vram->front();
    
    for (OEInt y = 0; y < vertDisplayedCell; y++)
    {
        wstring line;
        
        for (OEInt x = 0; x < horizDisplayed; x++)
            line += vp[y * horizDisplayed + x];
        
        line = rtrim(line);
        line += '\n';
        
        *s += line;
    }
}

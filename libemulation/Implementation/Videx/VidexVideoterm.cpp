
/**
 * libemulation
 * Videx Videoterm
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls a Videx Videoterm
 */

#include "VidexVideoterm.h"

#include "MemoryInterface.h"

#define CLOCK_FREQUENCY 17430000

#define FONT_CHARNUM    0x80
#define FONT_CHARWIDTH  8
#define FONT_CHARHEIGHT 16
#define FONT_SIZE       (FONT_CHARNUM * FONT_CHARWIDTH * FONT_CHARHEIGHT)

VidexVideoterm::VidexVideoterm() : MC6845()
{
    video = NULL;
    ram = NULL;
    
    ramBank = 0;
}

bool VidexVideoterm::setValue(string name, string value)
{
    if (name == "ramBank")
        ramBank = getOEInt(value);
    else if (name == "characterSet1")
        characterSet1 = value;
    else if (name == "characterSet2")
        characterSet2 = value;
    else if (name == "videoClockFrequency")
        videoClockFrequency = getFloat(value);
    else
        return MC6845::setValue(name, value);
    
    return true;
}

bool VidexVideoterm::getValue(string name, string &value)
{
    if (name == "ramBank")
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
        video = ref;
    else if (name == "ram")
        ram = ref;
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
    
    return true;
}

OEChar VidexVideoterm::read(OEAddress address)
{
    setRAMBank((address >> 2) & 0x3);
    
    return MC6845::read(address);
}

void VidexVideoterm::write(OEAddress address, OEChar value)
{
    setRAMBank((address >> 2) & 0x3);
    
    MC6845::write(address, value);
}

bool VidexVideoterm::loadFont(string name, OEData *data)
{
    if (data->size() < (FONT_SIZE * FONT_CHARWIDTH * FONT_CHARHEIGHT))
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
                theFont[(i * FONT_CHARHEIGHT + y) * FONT_CHARWIDTH + x] = (value & 0x80) ? 0xff : 0x00;
                
                value >>= 1;
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

void VidexVideoterm::updateTimer()
{
//    OEInt frameTime = horizTotal + vertTotal * scanline + vertTotalAdjust;
}

// To-Do:
// * Determine display rate
// * Request timer notification
// * Inhibit video
// * Draw screen image

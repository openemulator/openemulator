
/**
 * libemulation
 * MC6845
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MC6845 CRT controller
 */

#include "MC6845.h"

#include "ControlBusInterface.h"

typedef enum
{
    MC6845_HORIZTOTAL,
    MC6845_HORIZDISPLAYED,
    MC6845_HORIZSYNCPOSITION,
    MC6845_HORIZSYNCWIDTH,
    MC6845_VERTTOTAL,
    MC6845_VERTTOTALADJUST,
    MC6845_VERTDISPLAYED,
    MC6845_VERTSYNCPOSITION,
    MC6845_MODECONTROL,
    MC6845_SCANLINE,
    MC6845_CURSORSTART,
    MC6845_CURSOREND,
    MC6845_STARTADDRESSHIGH,
    MC6845_STARTADDRESSLOW,
    MC6845_CURSORADDRESSHIGH,
    MC6845_CURSORADDRESSLOW,
    MC6845_LIGHTPENHIGH,
    MC6845_LIGHTPENLOW,
} MC6845Register;

MC6845::MC6845()
{
    floatingBus = NULL;
    
    horizTotal = 0x65;
    horizDisplayed = 0x50;
    horizSyncPosition = 0x56;
    horizSyncWidth = 0x09;
    vertTotal = 0x18;
    vertTotalAdjust = 0x0a;
    vertDisplayed = 0x18;
    vertSyncPosition = 0x18;
    modeControl = 0x00;
    scanline = 0x0b;
    cursorStart = 0x00;
    cursorEnd = 0x0b;
    startAddress = 0x0080;
    cursorAddress = 0x0080;
    
    addressRegister = 0x00;
}

bool MC6845::setValue(string name, string value)
{
    if (name == "horizTotal")
        horizTotal = getOEInt(value);
    else if (name == "horizDisplayed")
        horizDisplayed = getOEInt(value);
    else if (name == "horizSyncPosition")
        horizSyncPosition = getOEInt(value);
    else if (name == "horizSyncWidth")
        horizSyncWidth = getOEInt(value);
    else if (name == "vertTotal")
        vertTotal = getOEInt(value);
    else if (name == "vertTotalAdjust")
        vertTotalAdjust = getOEInt(value);
    else if (name == "vertDisplayed")
        vertDisplayed = getOEInt(value);
    else if (name == "vertSyncPosition")
        vertSyncPosition = getOEInt(value);
    else if (name == "modeControl")
        modeControl = getOEInt(value);
    else if (name == "scanline")
        scanline = getOEInt(value);
    else if (name == "cursorStart")
        cursorStart = getOEInt(value);
    else if (name == "cursorEnd")
        cursorEnd = getOEInt(value);
    else if (name == "startAddress")
        startAddress = getOEInt(value);
    else if (name == "cursorAddress")
        cursorAddress = getOEInt(value);
    else if (name == "addressRegister")
        addressRegister = getOEInt(value);
    else
        return false;
    
    return true;
}

bool MC6845::getValue(string name, string& value)
{
    if (name == "horizTotal")
        value = getHexString(horizTotal);
    else if (name == "horizDisplayed")
        value = getHexString(horizDisplayed);
    else if (name == "horizSyncPosition")
        value = getHexString(horizSyncPosition);
    else if (name == "horizSyncWidth")
        value = getHexString(horizSyncWidth);
    else if (name == "vertTotal")
        value = getHexString(vertTotal);
    else if (name == "vertTotalAdjust")
        value = getHexString(vertTotalAdjust);
    else if (name == "vertDisplayed")
        value = getHexString(vertDisplayed);
    else if (name == "vertSyncPosition")
        value = getHexString(vertSyncPosition);
    else if (name == "modeControl")
        value = getHexString(modeControl);
    else if (name == "scanline")
        value = getHexString(scanline);
    else if (name == "cursorStart")
        value = getHexString(cursorStart);
    else if (name == "cursorEnd")
        value = getHexString(cursorEnd);
    else if (name == "startAddress")
        value = getHexString(startAddress);
    else if (name == "cursorAddress")
        value = getHexString(cursorAddress);
    else if (name == "addressRegister")
        value = getString(addressRegister);
    else
        return false;
    
    return true;
}

bool MC6845::setRef(string name, OEComponent *ref)
{
    if (name == "floatingBus")
        floatingBus = ref;
    else
        return false;
    
    return true;
}

bool MC6845::init()
{
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    return true;
}

OEChar MC6845::read(OEAddress address)
{
    if (!(address & 0x1))
        return floatingBus->read(address);
    
    switch (addressRegister)
    {
        case MC6845_CURSORADDRESSHIGH:
            return ((cursorAddress >> 8) & 0xff);
            
        case MC6845_CURSORADDRESSLOW:
            return ((cursorAddress >> 0) & 0xff);
        
        case MC6845_LIGHTPENHIGH:
            return ((lightpenAddress >> 8) & 0xff);
            
        case MC6845_LIGHTPENLOW:
            return ((lightpenAddress >> 0) & 0xff);
            
        default:
            return floatingBus->read(address);
    }
}

void MC6845::write(OEAddress address, OEChar value)
{
    if (!(address & 0x1))
    {
        addressRegister = value & 0x1f;
        
        return;
    }
    
    switch (addressRegister)
    {
        case MC6845_HORIZTOTAL:
            horizTotal = value;
            
            return;
            
        case MC6845_HORIZDISPLAYED:
            horizDisplayed = value;
            
            return;
            
        case MC6845_HORIZSYNCPOSITION:
            horizSyncPosition = value;
            
            return;
            
        case MC6845_HORIZSYNCWIDTH:
            horizSyncWidth = value & 0x0f;
            
            return;
            
        case MC6845_VERTTOTAL:
            vertTotal = value & 0x7f;
            
            return;
            
        case MC6845_VERTTOTALADJUST:
            vertTotalAdjust = value & 0x1f;
            
            return;
            
        case MC6845_VERTDISPLAYED:
            vertDisplayed = value & 0x7f;
            
            return;
            
        case MC6845_VERTSYNCPOSITION:
            vertSyncPosition = value & 0x7f;
            
            return;
            
        case MC6845_MODECONTROL:
            modeControl = value & 0x03;
            
            return;
            
        case MC6845_SCANLINE:
            scanline = value & 0x1f;
            
            return;
            
        case MC6845_CURSORSTART:
            cursorStart = value & 0x7f;
            
            return;
            
        case MC6845_CURSOREND:
            cursorEnd = value & 0x1f;
            
            return;
            
        case MC6845_STARTADDRESSHIGH:
            startAddress &= 0x00ff;
            startAddress |= (value << 8);
            
            return;
            
        case MC6845_STARTADDRESSLOW:
            startAddress &= 0xff00;
            startAddress |= (value << 0);
            
            return;
            
        case MC6845_CURSORADDRESSHIGH:
            cursorAddress &= 0x00ff;
            cursorAddress |= (value << 8);
            
            return;
            
        case MC6845_CURSORADDRESSLOW:
            cursorAddress &= 0xff00;
            cursorAddress |= (value << 0);
            
            return;
            
        default:
            return;
    }
}

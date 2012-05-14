
/**
 * libemulation
 * MC6845
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MC6845 CRT controller
 */

#include "MC6845.h"

#include "DeviceInterface.h"

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
    device = NULL;
    controlBus = NULL;
    floatingBus = NULL;
    monitorDevice = NULL;
    
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
    
    draw = &MC6845::dummyDraw;
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
    if (name == "device")
        device = ref;
    else if (name == "controlBus")
    {
        if (controlBus)
        {
            controlBus->removeObserver(this, CONTROLBUS_SCHEDULE_TIMER);
            controlBus->removeObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        }
        controlBus = ref;
        if (controlBus)
        {
            controlBus->addObserver(this, CONTROLBUS_SCHEDULE_TIMER);
            controlBus->addObserver(this, CONTROLBUS_POWERSTATE_DID_CHANGE);
        }
    }
    else if (name == "floatingBus")
        floatingBus = ref;
    else if (name == "monitorDevice")
    {
        if (monitorDevice)
            monitorDevice->removeObserver(this, DEVICE_DID_CHANGE);
        monitorDevice = ref;
        if (monitorDevice)
            monitorDevice->addObserver(this, DEVICE_DID_CHANGE);
    }
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
    
    float controlBusClockFrequency;
    controlBus->postMessage(this, CONTROLBUS_GET_CLOCKFREQUENCY, &controlBusClockFrequency);
    
    float videoClockFrequency = 17430000 / 9; // from Videx Videoterm 
    videoClockMultiplier = videoClockFrequency / controlBusClockFrequency;
    
    update();
    
    scheduleTimer(0);
    
    return true;
}

void MC6845::update()
{
    updateTiming();
}

void MC6845::notify(OEComponent *sender, int notification, void *data)
{
    if (sender == controlBus)
    {
        switch (notification)
        {
            case CONTROLBUS_POWERSTATE_DID_CHANGE:
                powerState = *((ControlBusPowerState *)data);
                
                break;
                
            case CONTROLBUS_TIMER_DID_FIRE:
                scheduleTimer(*((OESLong *)data));
                
                break;
        }
    }
    else if (sender == monitorDevice)
        device->postNotification(sender, notification, data);
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
            updateTiming();
            
            return;
            
        case MC6845_HORIZDISPLAYED:
            horizDisplayed = value;
            updateTiming();
            
            return;
            
        case MC6845_HORIZSYNCPOSITION:
            horizSyncPosition = value;
            updateTiming();
            
            return;
            
        case MC6845_HORIZSYNCWIDTH:
            horizSyncWidth = value & 0x0f;
            
            return;
            
        case MC6845_VERTTOTAL:
            vertTotal = value & 0x7f;
            updateTiming();
            
            return;
            
        case MC6845_VERTTOTALADJUST:
            vertTotalAdjust = value & 0x1f;
            updateTiming();
            
            return;
            
        case MC6845_VERTDISPLAYED:
            vertDisplayed = value & 0x7f;
            updateTiming();
            
            return;
            
        case MC6845_VERTSYNCPOSITION:
            vertSyncPosition = value & 0x7f;
            updateTiming();
            
            return;
            
        case MC6845_MODECONTROL:
            modeControl = value & 0x03;
            updateTiming();
            
            return;
            
        case MC6845_SCANLINE:
            scanline = value & 0x1f;
            updateTiming();
            
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

void MC6845::updateTiming()
{
    // This comes from VidexVideoterm:
    totalRect = OEMakeRect(0, 0,
                           (horizTotal + 1), (vertTotal + 1) * scanline + vertTotalAdjust);
    displayRect = OEMakeRect(horizSyncPosition, vertSyncPosition,
                             (horizDisplayed + 1), vertDisplayed + 1);
    
    controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, this);
    
    scheduleTimer(0);
}

void MC6845::scheduleTimer(OESLong cycles)
{
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &frameStart);
    frameStart += cycles;
    
    cycles += OEWidth(totalRect) * OEHeight(totalRect) * videoClockMultiplier;
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
}

void MC6845::setNeedsDisplay()
{
    pendingCycles = OEWidth(totalRect) * OEHeight(totalRect);
}

void MC6845::updateVideo()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    cycles /= videoClockMultiplier;
    
    OEInt deltaCycles = (OEInt) (cycles - lastCycles);
    
    OEInt cycleNum = min(pendingCycles, deltaCycles);
    
    if (cycleNum)
    {
        pendingCycles -= cycleNum;
        
        OEInt segmentStart = (OEInt) (lastCycles - frameStart);
        
        OEIPoint p0 = segment[segmentStart];
        OEIPoint p1 = segment[segmentStart + cycleNum];
        
        if (p0.y == p1.y)
            (this->*draw)(0, 0, p0.y, p0.x, p1.x, 0);
        else
        {
            (this->*draw)(0, 0, p0.y, p0.x, 65 - 1, 0);
            
            for (OEInt i = (p0.y + 1); i < p1.y; i++)
                (this->*draw)(0, 0, i, 0, 65 - 1, 0);
            
            (this->*draw)(0, 0, p1.y, 0, p1.x, 0);
        }
        
        imageModified = true;
    }
    
    lastCycles = cycles;
}

void MC6845::dummyDraw(OEInt memoryAddress, OEInt rasterAddress, OESInt y, OESInt x0, OESInt x1, OESInt cursor)
{
}


/**
 * libemulation
 * MC6845
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MC6845 CRT controller
 */

#include <math.h>

#include "MC6845.h"

#include "CanvasInterface.h"

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

#define BLINK_PERIOD    (1 << 5)
#define BLINK_ENABLED   (1 << 6)

MC6845::MC6845()
{
    controlBus = NULL;
    floatingBus = NULL;
    
    horizTotal = 1;
    horizDisplayed = 0;
    horizSyncPosition = 0;
    horizSyncWidth = 0;
    vertTotalCell = 1;
    vertTotalAdjust = 0;
    vertDisplayedCell = 0;
    vertSyncPositionCell = 0;
    modeControl = 0;
    scanline = 1;
    setCursorStart(0);
    cursorEnd = 0;
    startAddress.q = 0;
    cursorAddress.q = 0;
    
    addressRegister = 0;
    
    imageModified = false;
    
    frameStart = 0;
    pendingCycles = 0;
    
    blinkCount = 0;
    
    powerState = CONTROLBUS_POWERSTATE_ON;
}

bool MC6845::setValue(string name, string value)
{
    if (name == "horizTotal")
        horizTotal = getOEInt(value) + 1;
    else if (name == "horizDisplayed")
        horizDisplayed = getOEInt(value);
    else if (name == "horizSyncPosition")
        horizSyncPosition = getOEInt(value);
    else if (name == "horizSyncWidth")
        horizSyncWidth = getOEInt(value);
    else if (name == "vertTotal")
        vertTotalCell = getOEInt(value) + 1;
    else if (name == "vertTotalAdjust")
        vertTotalAdjust = getOEInt(value);
    else if (name == "vertDisplayed")
        vertDisplayedCell = getOEInt(value);
    else if (name == "vertSyncPosition")
        vertSyncPositionCell = getOEInt(value);
    else if (name == "modeControl")
        modeControl = getOEInt(value);
    else if (name == "scanline")
        scanline = getOEInt(value) + 1;
    else if (name == "cursorStart")
        setCursorStart(getOEInt(value));
    else if (name == "cursorEnd")
        cursorEnd = getOEInt(value);
    else if (name == "startAddress")
        startAddress.q = getOEInt(value);
    else if (name == "cursorAddress")
        cursorAddress.q = getOEInt(value);
    else if (name == "addressRegister")
        addressRegister = getOEInt(value);
    else
        return false;
    
    return true;
}

bool MC6845::getValue(string name, string& value)
{
    if (name == "horizTotal")
        value = getHexString(horizTotal - 1);
    else if (name == "horizDisplayed")
        value = getHexString(horizDisplayed);
    else if (name == "horizSyncPosition")
        value = getHexString(horizSyncPosition);
    else if (name == "horizSyncWidth")
        value = getHexString(horizSyncWidth);
    else if (name == "vertTotal")
        value = getHexString(vertTotalCell - 1);
    else if (name == "vertTotalAdjust")
        value = getHexString(vertTotalAdjust);
    else if (name == "vertDisplayed")
        value = getHexString(vertDisplayedCell);
    else if (name == "vertSyncPosition")
        value = getHexString(vertSyncPositionCell);
    else if (name == "modeControl")
        value = getHexString(modeControl);
    else if (name == "scanline")
        value = getHexString(scanline - 1);
    else if (name == "cursorStart")
        value = getHexString(cursorStart);
    else if (name == "cursorEnd")
        value = getHexString(cursorEnd);
    else if (name == "startAddress")
        value = getHexString(startAddress.q);
    else if (name == "cursorAddress")
        value = getHexString(cursorAddress.q);
    else if (name == "addressRegister")
        value = getString(addressRegister);
    else
        return false;
    
    return true;
}

bool MC6845::setRef(string name, OEComponent *ref)
{
    if (name == "controlBus")
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
    else
        return false;
    
    return true;
}

bool MC6845::init()
{
    if (!controlBus)
    {
        logMessage("controlBus not connected");
        
        return false;
    }
    
    if (!floatingBus)
    {
        logMessage("floatingBus not connected");
        
        return false;
    }
    
    controlBus->postMessage(this, CONTROLBUS_GET_POWERSTATE, &powerState);
    
    updateTiming();
    
    return true;
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
}

OEChar MC6845::read(OEAddress address)
{
    if (!(address & 0x1))
        return floatingBus->read(address);
    
    switch (addressRegister)
    {
        case MC6845_CURSORADDRESSHIGH:
            return cursorAddress.b.h;
            
        case MC6845_CURSORADDRESSLOW:
            return cursorAddress.b.l;
            
        case MC6845_LIGHTPENHIGH:
            return lightpenAddress.b.h;
            
        case MC6845_LIGHTPENLOW:
            return lightpenAddress.b.l;
            
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
            horizTotal = value + 1;
            
            updateTiming();
            
            break;
            
        case MC6845_HORIZDISPLAYED:
            horizDisplayed = value;
            
            updateTiming();
            
            break;
            
        case MC6845_HORIZSYNCPOSITION:
            horizSyncPosition = value;
            
            updateTiming();
            
            break;
            
        case MC6845_HORIZSYNCWIDTH:
            horizSyncWidth = value & 0x0f;
            
            break;
            
        case MC6845_VERTTOTAL:
            vertTotalCell = (value & 0x7f) + 1;
            
            updateTiming();
            
            break;
            
        case MC6845_VERTTOTALADJUST:
            vertTotalAdjust = value & 0x1f;
            
            updateTiming();
            
            break;
            
        case MC6845_VERTDISPLAYED:
            vertDisplayedCell = value & 0x7f;
            
            updateTiming();
            
            break;
            
        case MC6845_VERTSYNCPOSITION:
            vertSyncPositionCell = value & 0x7f;
            
            updateTiming();
            
            break;
            
        case MC6845_MODECONTROL:
            modeControl = value & 0x03;
            
            updateTiming();
            
            break;
            
        case MC6845_SCANLINE:
            scanline = (value & 0x1f) + 1;
            
            updateTiming();
            
            break;
            
        case MC6845_CURSORSTART:
            refreshVideo();
            
            setCursorStart(value);
            
            break;
            
        case MC6845_CURSOREND:
            refreshVideo();
            
            cursorEnd = value & 0x1f;
            
            break;
            
        case MC6845_STARTADDRESSHIGH:
            refreshVideo();
            
            startAddress.b.h = value & 0x3f;
            
            break;
            
        case MC6845_STARTADDRESSLOW:
            refreshVideo();
            
            startAddress.b.l = value & 0xff;
            
            break;
            
        case MC6845_CURSORADDRESSHIGH:
            refreshVideo();
            
            cursorAddress.b.h = value & 0x3f;
            
            break;
            
        case MC6845_CURSORADDRESSLOW:
            refreshVideo();
            
            cursorAddress.b.l = value & 0xff;
            
            break;
    }
}

void MC6845::setCursorStart(OEChar value)
{
    cursorStart = value & 0x7f;
    
    blinkEnabled = OEGetBit(value, BLINK_ENABLED);
    blink = OEGetBit(value, BLINK_PERIOD);
    blinkFrameNum = OEGetBit(value, BLINK_PERIOD) ? 32 : 16;
}

void MC6845::updateTiming()
{
    float controlBusClockFrequency;
    controlBus->postMessage(this, CONTROLBUS_GET_CLOCKFREQUENCY, &controlBusClockFrequency);
    clockMultiplier = clockFrequency / controlBusClockFrequency;
    
    vertTotal = vertTotalCell * scanline + vertTotalAdjust;
    vertDisplayed = vertDisplayedCell * scanline;
    vertSyncPosition = vertSyncPositionCell * scanline;
    
    frameCycleNum = horizTotal * vertTotal;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &lastCycles);
    
    controlBus->postMessage(this, CONTROLBUS_INVALIDATE_TIMERS, this);
    
    scheduleTimer(0);
    
    refreshVideo();
}

void MC6845::scheduleTimer(OESLong cycles)
{
    updateVideo();
    
    if (imageModified)
    {
        imageModified = false;
        
        if (powerState != CONTROLBUS_POWERSTATE_OFF)
            postImage();
    }
    
    if ((powerState == CONTROLBUS_POWERSTATE_ON) && blinkEnabled)
    {
        blinkCount++;
        
        if (blinkCount >= blinkFrameNum)
        {
            blink = !blink;
            blinkCount = 0;
            
            refreshVideo();
        }
    }
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &frameStart);
    frameStart += cycles;
    
    cycles += ceil(frameCycleNum / clockMultiplier);
    
    controlBus->postMessage(this, CONTROLBUS_SCHEDULE_TIMER, &cycles);
}

void MC6845::refreshVideo()
{
    updateVideo();
    
    pendingCycles = frameCycleNum;
}

void MC6845::updateVideo()
{
    OELong cycles;
    
    controlBus->postMessage(this, CONTROLBUS_GET_CYCLES, &cycles);
    
    OEInt deltaCycles = (OEInt) ((cycles - lastCycles) * clockMultiplier);
    
    OEInt cycleNum = min(pendingCycles, deltaCycles);
    
    if (cycleNum)
    {
        pendingCycles -= cycleNum;
        
        OEInt segmentStart = (OEInt) ((lastCycles - frameStart) * clockMultiplier);
        
        OEIntPoint p0 = pos[segmentStart];
        OEIntPoint p1 = pos[segmentStart + cycleNum];
        
        if (p0.y == p1.y)
            (this->*draw)(p0.y, p0.x, p1.x);
        else
        {
            (this->*draw)(p0.y, p0.x, posXEnd);
            
            for (OEInt i = (p0.y + 1); i < p1.y; i++)
                (this->*draw)(i, posXBegin, posXEnd);
            
            (this->*draw)(p1.y, posXBegin, p1.x);
        }
        
        imageModified = true;
    }
    
    lastCycles = cycles;
}

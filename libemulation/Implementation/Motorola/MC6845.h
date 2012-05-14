
/**
 * libemulation
 * MC6845
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a MC6845 CRT controller
 */

#ifndef _MC6845_H
#define _MC6845_H

#include "OEComponent.h"
#include "OEImage.h"

#include "ControlBusInterface.h"

class MC6845 : public OEComponent
{
public:
    MC6845();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
protected:
    void (MC6845::*draw)(OEInt memoryAddress, OEInt rasterAddress, OESInt y, OESInt x0, OESInt x1, OESInt cursor);
    
private:
    OEComponent *device;
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *monitorDevice;
    
    OEChar horizTotal;
    OEChar horizDisplayed;
    OEChar horizSyncPosition;
    OEChar horizSyncWidth;
    OEChar vertTotal;
    OEChar vertTotalAdjust;
    OEChar vertDisplayed;
    OEChar vertSyncPosition;
    OEChar modeControl;
    OEChar scanline;
    OEChar cursorStart;
    OEChar cursorEnd;
    OEShort startAddress;
    OEShort cursorAddress;
    OEShort lightpenAddress;
    
    OEChar addressRegister;
    
    ControlBusPowerState powerState;
    
    OERect totalRect;
    OERect visibleRect;
    OERect displayRect;
    
    float videoClockMultiplier;
    OELong frameStart;
    
    OELong lastCycles;
    OEInt pendingCycles;
    
    vector<OEIPoint> segment;
    bool imageModified;
    
    void updateTiming();
    
    void scheduleTimer(OESLong cycles);
    
    void setNeedsDisplay();
    void updateVideo();
    void dummyDraw(OEInt memoryAddress, OEInt rasterAddress, OESInt y, OESInt x0, OESInt x1, OESInt cursor);
};

#endif

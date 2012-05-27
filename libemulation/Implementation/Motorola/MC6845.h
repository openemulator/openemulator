
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

class MC6845;

typedef void (MC6845::*MC6845Draw)(OESInt y, OESInt x0, OESInt x1);

class MC6845 : public OEComponent
{
public:
    MC6845();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    
    void notify(OEComponent *sender, int notification, void *data);
    
    OEChar read(OEAddress address);
    void write(OEAddress address, OEChar value);
    
protected:
    OEInt horizTotal;
    OEInt horizDisplayed;
    OEInt horizSyncPosition;
    OEInt horizSyncWidth;
    OEInt vertTotalCell;
    OEInt vertTotalAdjust;
    OEInt vertDisplayedCell;
    OEInt vertSyncPositionCell;
    OEInt modeControl;
    OEInt scanline;
    OEInt cursorStart;
    OEInt cursorEnd;
    OEUnion startAddress;
    OEUnion lightpenAddress;
    OEUnion cursorAddress;
    
    OEInt vertTotal;
    OEInt vertDisplayed;
    OEInt vertSyncPosition;
    
    vector<OEIntPoint> pos;
    OESInt posXBegin;
    OESInt posXEnd;
    
    float clockFrequency;
    
    OEInt frameCycleNum;
    
    bool blink;
    
    MC6845Draw draw;
    
    void refreshVideo();
    
    virtual void updateTiming();
    virtual void postImage() = 0;
    
private:
    OEComponent *controlBus;
    OEComponent *floatingBus;
    
    OEChar addressRegister;
    
    // State
    bool imageModified;
    
    // Timing
    float clockMultiplier;
    
    OELong frameStart;
    
    OELong lastCycles;
    OEInt pendingCycles;
    
    OEInt blinkFrameNum;
    OEInt blinkCount;
    bool blinkEnabled;
    
    ControlBusPowerState powerState;
    
    void setCursorStart(OEChar value);
    void scheduleTimer(OESLong cycles);
    
    void updateVideo();
};

#endif

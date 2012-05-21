
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
    OEChar horizTotal;
    OEChar horizDisplayed;
    OEChar horizSyncPosition;
    OEChar horizSyncWidth;
    OEChar vertTotalCell;
    OEChar vertTotalAdjust;
    OEChar vertDisplayedCell;
    OEChar vertSyncPositionCell;
    OEChar modeControl;
    OEChar scanline;
    OEChar cursorStart;
    OEChar cursorEnd;
    OEUnion startAddress;
    OEUnion lightpenAddress;
    OEUnion cursorAddress;
    
    OEInt vertTotal;
    OEInt vertDisplayed;
    OEInt vertSyncPosition;
    
    vector<OECount> pos;
    OESInt posBegin;
    OESInt posEnd;
    
    float clockFrequency;
    
    OEInt frameCycleNum;
    
    MC6845Draw draw;
    
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
    
    bool blink;
    OEInt blinkFrameNum;
    OEInt blinkCount;
    bool blinkEnabled;
    
    ControlBusPowerState powerState;
    
    void scheduleTimer(OESLong cycles);
    
    void refreshVideo();
    void updateVideo();
};

#endif


/**
 * libemulation
 * Apple III Video
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements Apple III Video
 */

#ifndef _APPLEIIIVIDEO_H
#define _APPLEIIIVIDEO_H

#include "OEComponent.h"
#include "OEImage.h"

#include "ControlBusInterface.h"

class AppleIIIVideo : public OEComponent
{
public:
    AppleIIIVideo();
    
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
    bool getData(string name, OEData **data);
    bool init();
    void update();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEComponent *controlBus;
    OEComponent *systemControl;
    OEComponent *vram;
    OEComponent *monochromeMonitor;
    OEComponent *compositeMonitor;
    OEComponent *rgbMonitor;
    
    // Settings
    OEInt flashFrameNum;
    OEInt mode;
    
    bool revisionUpdated;
    bool videoSystemUpdated;
    
    // Tables
    vector<OEIntPoint> pos;
    vector<OEIntPoint> count;
    
    vector<OEInt> textOffset;
    vector<OEInt> hiresOffset;
    
    // Memory
    OEChar *vramp;
    
    OEData characterSet;
    
    // Drawing
    bool videoEnabled;
    bool colorKiller;
    
    OEData text40Font;
    OEData text80Font;
    OEData loresFont;
    OEData hires40Font;
    OEData hires80Font;
    
    OEData framebuffer;
    OEChar *framebufferp;
    bool framebufferModified;
    
    void (AppleIIIVideo::*draw)(OESInt y, OESInt x0, OESInt x1);
    OEChar *drawMemory1;
    OEChar *drawMemory2;
    OEChar *drawFont;
    
    // Timing
    OEInt vertTotal;
    OEInt vertStart;
    
    OELong frameStart;
    OEInt frameCycleNum;
    
    OEInt currentTimer;
    OELong lastCycles;
    OEInt pendingCycles;
    
    bool flash;
    OEInt flashCount;
    
    // State
    ControlBusPowerState powerState;
    bool monitorConnected;
    OEInt videoInhibitCount;
    bool appleIIMode;
    
    void initOffsets();
    
    bool updateCharacterSet();
    bool loadTextFont(string name, OEData *data);
    void buildLoresFont();
    void buildHiresFont();
    
    void initVideoRAM(OEComponent *ram, OEAddress &start);
    void updateImage();
    void updateClockFrequency();
    
    void updateMonitorConnected();
    
    void setMode(OEInt mask, bool value);
    void configureDraw();
    void drawText40MLine(OESInt y, OESInt x0, OESInt x1);
    void drawText40CLine(OESInt y, OESInt x0, OESInt x1);
    void drawText80Line(OESInt y, OESInt x0, OESInt x1);
    void drawLoresLine(OESInt y, OESInt x0, OESInt x1);
    void drawHires40MLine(OESInt y, OESInt x0, OESInt x1);
    void drawHires40CLine(OESInt y, OESInt x0, OESInt x1);
    void drawHires80Line(OESInt y, OESInt x0, OESInt x1);
    void drawHires140Line(OESInt y, OESInt x0, OESInt x1);
    void updateVideoEnabled();
    void refreshVideo();
    void updateVideo();
    
    void updateTiming();
    void scheduleNextTimer(OESLong cycles);
    OEIntPoint getCount();
    OEChar readFloatingBus();
    
    void copy(wstring *s);
};

#endif

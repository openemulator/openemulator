
/**
 * libemulator
 * Apple II Video
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates Apple II video
 */

#include "OEComponent.h"
#include "OEImage.h"

#include "ControlBusInterface.h"

#include "AppleIIInterface.h"

typedef enum
{
    APPLEII_MODELII,
    APPLEII_MODELIIJPLUS,
    APPLEII_MODELIIE,
} AppleIIModel;

typedef enum
{
    APPLEII_NTSC,
    APPLEII_PAL,
} AppleIITVSystem;

typedef enum
{
    APPLEII_TIMER_VSYNC,
    APPLEII_TIMER_DISPLAYMIXED,
    APPLEII_TIMER_DISPLAYEND,
} AppleIITimerType;

class AppleIIVideo : public OEComponent
{
public:
    AppleIIVideo();
    
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
	bool init();
    void update();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
private:
    OEComponent *device;
    OEComponent *controlBus;
    OEComponent *memoryBus;
    OEComponent *floatingBus;
    OEComponent *gamePort;
    OEComponent *monitorDevice;
	OEComponent *monitor;
	
    // Settings
    AppleIIModel model;
    OEInt revision;
    AppleIITVSystem tvSystem;
	string characterSet;
    OEInt flashFrameNum;
    OEInt mode;
    
    // Tables
    vector<OEIPoint> segment;
    vector<OEIPoint> count;
    OESInt limit[66];
    
    vector<OEInt> textOffset;
    vector<OEInt> hiresOffset;
    
    map<string, OEData> textFont;
    OEData loresFont;
    OEData hiresFont;
    
    AppleIIVRAM vram;
    
    // State variables
    bool inhibitVideo;
    
    OEImage image;
    OEChar *imagep;
    bool imageModified;
    
    void (AppleIIVideo::*draw)(OESInt y, OESInt x0, OESInt x1);
    OEChar *drawMemory;
    OEChar *drawFont;
    
    OERect totalRect;
    OERect visibleRect;
    OERect displayRect;
    
    OELong frameStart;
    AppleIITimerType currentTimer;
    
    OELong lastCycles;
    OEInt pendingCycles;
    
    bool flashActive;
    OEInt flashCount;
    
    ControlBusPowerState powerState;
    
    bool an2;
    
    bool isRevisionUpdated;
    bool isTVSystemUpdated;
    
    void buildTables();
    void initOffsets();
    
    bool loadTextFont(string name, OEData *data);
    void buildLoresFont();
    void buildHiresFont();
    
    void initVideoRAM(OEComponent *ram, OEAddress &start);
    void updateImage();
    void updateClockFrequency();
    
    void setMode(OEInt mask, bool value);
    
    void setNeedsDisplay();
    void updateVideo();
    
    void configureDraw();
    void drawTextLine(OESInt y, OESInt x0, OESInt x1);
    void drawLoresLine(OESInt y, OESInt x0, OESInt x1);
    void drawHiresLine(OESInt y, OESInt x0, OESInt x1);
    
    void scheduleNextTimer(OESLong cycles);
    OEIPoint getCount();
    OEChar readFloatingBus();
    
    void vsync();
    
    void copy(wstring *s);
};

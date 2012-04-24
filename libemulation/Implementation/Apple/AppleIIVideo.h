
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

class AppleIIVideoPoint
{
public:
    AppleIIVideoPoint()
    {
        x = 0;
        y = 0;
    }
    
    AppleIIVideoPoint(int x, int y)
    {
        this->x = x;
        this->y = y;
    }
    
    int x;
    int y;
};

typedef enum
{
    APPLEII_MODELII,
    APPLEII_MODELIIJPLUS,
    APPLEII_MODELIIE,
} AppleIIModel;

typedef enum
{
    APPLEII_REVISION0,
    APPLEII_REVISION1,
} AppleIIRevision;

typedef enum
{
    APPLEII_NTSC,
    APPLEII_PAL,
} AppleIITVSystem;

typedef enum
{
    APPLEII_RENDERER_TEXT,
    APPLEII_RENDERER_LORES,
    APPLEII_RENDERER_HIRES,
} AppleIIRenderer;

typedef enum
{
    APPLEII_TIMER_ACTIVESTART,
    APPLEII_TIMER_MIXED,
    APPLEII_TIMER_ACTIVEEND,
    APPLEII_TIMER_VSYNC,
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
    AppleIIRevision revision;
    AppleIITVSystem tvSystem;
	string characterSet;
    OEInt flashFrameNum;
    OEInt mode;
    
    // Tables
    vector<int> segment;
    vector<AppleIIVideoPoint> point;
    vector<AppleIIVideoPoint> count;
    
    vector<OEInt> textOffset;
    vector<OEInt> hiresOffset;
    
    map<string, OEData> textMap;
    OEData loresMap;
    OEData hiresMap;
    
    AppleIIVRAM vram;
    
    // State variables
    OEImage image;
    bool imageDidChange;
    
    AppleIIRenderer renderer;
    OEChar *rendererImage;
    OEChar *rendererTextMemory;
    OEChar *rendererHiresMemory;
    OEChar *rendererTextMap;
    OEChar *rendererLoresMap;
    OEChar *rendererHiresMap;
    
    OERect videoRect;
    OERect pictureRect;
    OERect activeRect;
    
    OELong frameStart;
    AppleIITimerType currentTimer;
    int lastSegment;
    int pendingSegments;
    
    bool flashActive;
    OEInt flashCount;
    
    ControlBusPowerState powerState;
    
    bool an2;
    
    bool isRevisionUpdated;
    bool isTVSystemUpdated;
        
    void updateSegments();
    void initPoints();
    void updateCounts();
    void initOffsets();
    void loadTextMap(string name, OEData *data);
    void initLoresMap();
    void updateHiresMap();
    void initVideoRAM(OEComponent *ram, OEAddress &start);
    void updateImage();
    void updateClockFrequency();
    void updateRendererMap();
    void updateRenderer();
    
    void setMode(OEInt mask, bool value);
    
    void updateVideo();
    void drawVideoLine(int y, int x0, int x1);
    void setNeedsDisplay();
        
    void scheduleNextTimer(OESLong cycles);
    AppleIIVideoPoint getCount();
    OEChar readFloatingBus();
    
    void vsync();
    
    void copy(wstring *s);
};

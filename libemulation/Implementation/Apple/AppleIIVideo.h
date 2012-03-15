
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
    
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
    OEComponent *device;
    OEComponent *controlBus;
    OEComponent *mmu;
    OEComponent *floatingBus;
    OEComponent *ram1;
    OEComponent *ram2;
    OEComponent *ram3;
    OEComponent *gamePort;
    OEComponent *monitorDevice;
	OEComponent *monitor;
	
    // Settings
    AppleIIModel model;
    AppleIIRevision revision;
    AppleIITVSystem tvSystem;
	string characterSet;
    OEUInt32 flashFrameNum;
    OEUInt32 mode;
    
    // Tables
    vector<int> segment;
    vector<AppleIIVideoPoint> point;
    vector<AppleIIVideoPoint> count;
    
    vector<OEAddress> textOffset;
    vector<OEAddress> hiresOffset;
    
    map<string, OEData> textMap;
    OEData loresMap;
    OEData hiresMap;
    
    OEUInt8 *textMemory[2];
    OEUInt8 *textHBLMemory[2];
    OEUInt8 *hiresMemory[2];
    
    // State variables
    OEImage image;
    bool imageDidChange;
    
    AppleIIRenderer renderer;
    OEUInt8 *rendererImage;
    OEUInt8 *rendererTextMemory;
    OEUInt8 *rendererHiresMemory;
    OEUInt8 *rendererTextMap;
    OEUInt8 *rendererLoresMap;
    OEUInt8 *rendererHiresMap;
    
    OERect videoRect;
    OERect pictureRect;
    OERect activeRect;
    
    OEUInt64 frameStart;
    AppleIITimerType currentTimer;
    int lastSegment;
    int pendingSegments;
    
    bool flashActive;
    OEUInt32 flashCount;
    
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
    
    void setMode(OEUInt32 mask, bool value);
    
    void updateVideo();
    void drawVideoLine(int y, int x0, int x1);
    void setNeedsDisplay();
    
    void initVideoRAMSync();
    
    void scheduleNextTimer(OEInt64 cycles);
    AppleIIVideoPoint getCount();
    OEUInt8 readFloatingBus();
    
    void vsync();
    
    void copy(wstring *s);
};

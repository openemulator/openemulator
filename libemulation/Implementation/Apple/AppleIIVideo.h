
/**
 * libemulator
 * Apple II Video
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
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
    APPLEIIVIDEO_REVISION0,
    APPLEIIVIDEO_REVISION1,
    APPLEIIVIDEO_REVISIONIIE,
} AppleIIVideoRevision;

typedef enum
{
    APPLEIIVIDEO_NTSC,
    APPLEIIVIDEO_PAL,
} AppleIIVideoSystem;

typedef enum
{
    APPLEIIVIDEO_RENDERER_TEXT,
    APPLEIIVIDEO_RENDERER_LORES,
    APPLEIIVIDEO_RENDERER_HIRES,
} AppleIIVideoRenderer;

typedef enum
{
    APPLEIIVIDEO_TIMER_ACTIVESTART,
    APPLEIIVIDEO_TIMER_MIXED,
    APPLEIIVIDEO_TIMER_ACTIVEEND,
    APPLEIIVIDEO_TIMER_VSYNC,
} AppleIIVideoTimerType;

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
    OEComponent *videoRAMSync;
    OEComponent *monitorDevice;
	OEComponent *monitor;
	
    // Settings
    AppleIIVideoRevision revision;
    AppleIIVideoSystem tvSystem;
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
    
    AppleIIVideoRenderer renderer;
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
    AppleIIVideoTimerType currentTimer;
    int lastSegment;
    int pendingSegments;
    
    bool flashActive;
    OEUInt32 flashCount;
    
    ControlBusPowerState powerState;
    
    bool isRevisionUpdated;
    bool isTVSystemUpdated;
    
    bool isVideoRAMInSync;
    
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
    
    void updateVideoRAMSync();
    
    void scheduleNextTimer(OEInt64 cycles);
    AppleIIVideoPoint getCount();
    OEUInt8 readFloatingBus();
    
    void vsync();
    
    void copy(wstring *s);
};


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

typedef struct {
    OEUInt32 h;
    OEUInt32 v;
} AppleIIVideoCount;

typedef enum
{
    APPLEIIVIDEO_RENDERER_TEXT,
    APPLEIIVIDEO_RENDERER_LORES,
    APPLEIIVIDEO_RENDERER_HIRES,
} AppleIIVideoRenderer;

class AppleIIVideo : public OEComponent
{
public:
    AppleIIVideo();
    
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
	bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
    OEComponent *device;
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *ram1;
    OEComponent *ram2;
    OEComponent *ram3;
    OEComponent *monitorDevice;
	OEComponent *monitor;
	
    bool rev0;
	bool palTiming;
	string characterSet;
    
    OEUInt32 mode;
    
    AppleIIVideoRenderer renderer;
    
    bool vsyncTimer;
    OEUInt64 vsyncCycleCount;
    
    bool canvasShouldUpdate;
    AppleIIVideoCount lastCount;
    
    OEUInt8 *textMemory[2];
    OEUInt8 *hiresMemory[2];
    vector<OEAddress> textOffset;
    vector<OEAddress> hiresOffset;
    
    map<string, OEData> textMap;
    OEData loresMap;
    
    OEImage image;
    bool flash;
    OEUInt32 flashCount;
    
    ControlBusPowerState powerState;
    
    void getVRAM(OEComponent *ram, OEAddress &start);
    void scheduleTimer();
    
    void initOffsets();
    void loadTextMap(string name, OEData *data);
    void initLoresMap();
    
    void setMode(OEUInt32 mask, bool value);
    AppleIIVideoCount getCount();
    OEUInt8 readFloatingBus();
    
    void vsync();
    
    void updateVideo();
    void drawText();
    void drawLores();
    void drawHires();
    void copy(wstring *s);
};

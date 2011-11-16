
/**
 * libemulator
 * Apple II Video Generator
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Generates Apple II video
 */

#include "OEComponent.h"
#include "OEImage.h"

#include "CanvasInterface.h"
#include "ControlBusInterface.h"
#include "AppleIIInterface.h"

class AppleIIVideo : public OEComponent
{
public:
    AppleIIVideo();
    
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
	bool init();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
    OEComponent *device;
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *mmu;
    OEComponent *monitorDevice;
	OEComponent *monitor;
	
    bool rev0;
	bool palTiming;
	string characterSet;
    
    OEUInt32 mode;
    
    AppleIIMMUVideoMemory vram;
    OEUInt32 cursorX, cursorY;
    
    map<string, OEData> font;
    bool canvasShouldUpdate;
    OEImage image;
    bool cursorActive;
    OEUInt32 cursorCount;
    
    ControlBusPowerState powerState;
    
    void updateMode(OEUInt32 mask, bool value);
    void scheduleTimer();
    OEData loadFont(OEData *data);
    void vsync();
    void copy(wstring *s);
};

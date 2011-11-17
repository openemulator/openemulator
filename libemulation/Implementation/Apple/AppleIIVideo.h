
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

#include "ControlBusInterface.h"

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
    OEUInt8 *text[2];
    OEUInt8 *hires[2];
    map<string, OEData> font;
    bool canvasShouldUpdate;
    OEImage image;
    bool flash;
    OEUInt32 flashCount;
    
    ControlBusPowerState powerState;
    
    void getVRAM(OEComponent *ram, OEAddress &start);
    void scheduleTimer();
    void loadFont(string name, OEData *data);
    void updateMode(OEUInt32 mask, bool value);
    void vsync();
    void copy(wstring *s);
};

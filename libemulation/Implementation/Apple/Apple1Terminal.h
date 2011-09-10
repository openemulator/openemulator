
/**
 * libemulation
 * Apple I Terminal
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple I Terminal
 */

#include "OEComponent.h"

#include "OEImage.h"
#include "CanvasInterface.h"
#include "ControlBus.h"

class Apple1Terminal : public OEComponent
{
public:
    Apple1Terminal();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
    bool init();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
    void write(OEAddress address, OEUInt8 value);
    
private:
    OEComponent *device;
    OEComponent *vram;
    OEComponent *controlBus;
    OEComponent *monitorDevice;
    OEComponent *monitor;
    
    bool cursorActive;
    OEUInt32 cursorCount;
    OEUInt32 cursorX, cursorY;
    
    OEData font;
    char *vramp;
    OEImage image;
    
    ControlBusPowerState powerState;
    
    void scheduleTimer();
    void loadFont(OEData *data);
    void updateCanvas();
    void updateBezel();
    void sendUnicodeChar(CanvasUnicodeChar unicodeChar);
    void copy(string *s);
    void paste(string *s);
};

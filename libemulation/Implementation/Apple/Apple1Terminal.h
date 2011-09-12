
/**
 * libemulation
 * Apple-1 Terminal
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 terminal
 */

#include "OEComponent.h"

#include "OEImage.h"
#include "CanvasInterface.h"
#include "ControlBus.h"

#include <queue>

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
    
    queue<OEUInt8> keyQueue;
    bool isRTS;
    
    void scheduleTimer();
    void loadFont(OEData *data);
    void updateCanvas();
    void updateBezel();
    void putChar(OEUInt8 c);
    void clearScreen();
    void enqueueKey(OEUInt8 c);
    void emptyQueue();
    void copy(wstring *s);
    void paste(wstring *s);
};

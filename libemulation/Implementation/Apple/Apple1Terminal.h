
/**
 * libemulation
 * Apple-1 Terminal
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an Apple-1 terminal
 */

#include <queue>

#include "OEComponent.h"

#include "OEImage.h"
#include "CanvasInterface.h"
#include "ControlBusInterface.h"

class Apple1Terminal : public OEComponent
{
public:
    Apple1Terminal();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool setData(string name, OEData *data);
    bool init();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    OEUInt32 cursorX, cursorY;
    bool clearScreenOnCtrlL;
    bool splashScreen;
    bool splashScreenActive;
    
    OEComponent *device;
    OEComponent *controlBus;
    OEComponent *vram;
    OEComponent *monitorDevice;
    OEComponent *monitor;
    
    OEUInt8 *vramp;
    OEData font;
    bool canvasShouldUpdate;
    OEImage image;
    bool cursorActive;
    OEUInt32 cursorCount;
    
    ControlBusPowerState powerState;
        
    bool isRTS;
    queue<OEUInt8> pasteBuffer;
    
    void scheduleTimer();
    void loadFont(OEData *data);
    void vsync();
    void clearScreen();
    void putChar(OEUInt8 c);
    void sendKey(CanvasUnicodeChar key);
    void copy(wstring *s);
    void paste(wstring *s);
    void emptyPasteBuffer();
};

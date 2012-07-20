
/**
 * libemulation
 * Apple-1 Terminal
 * (C) 2010-2012 by Marc S. Ressl (mressl@umich.edu)
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
    void update();
    void dispose();
    
    bool postMessage(OEComponent *sender, int message, void *data);
    
    void notify(OEComponent *sender, int notification, void *data);
    
private:
    // Settings
    OEInt cursorX, cursorY;
    bool clearScreenOnCtrlL;
    bool splashScreen;
    bool splashScreenActive;
    
    OEComponent *emulation;
    OEComponent *controlBus;
    OEComponent *vram;
    OEComponent *monitor;
    
    // State
    OEChar *vramp;
    OEData font;
    bool updateCanvas;
    
    OEImage image;
    
    bool cursorActive;
    OEInt cursorCount;
    
    ControlBusPowerState powerState;
    
    bool isRTS;
    queue<OEChar> pasteBuffer;
    
    void loadFont(OEData *data);
    
    void scheduleNextTimer(OESLong cycles);
    void drawFrame();
    
    void clearScreen();
    void putChar(OEChar c);
    void sendKey(CanvasUnicodeChar key);
    
    void copy(wstring *s);
    void paste(wstring *s);
    void emptyPasteBuffer();
};

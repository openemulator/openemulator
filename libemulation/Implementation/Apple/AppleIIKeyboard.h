
/**
 * libemulator
 * Apple II Keyboard
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II keyboard
 */

#ifndef _APPLEIIKEYBOARD_H
#define _APPLEIIKEYBOARD_H

#include "OEComponent.h"

#include "CanvasInterface.h"

#include <queue>

typedef enum
{
    APPLEIIKEYBOARD_TYPE_STANDARD,
    APPLEIIKEYBOARD_TYPE_SHIFTKEYMOD,
    APPLEIIKEYBOARD_TYPE_FULLASCII,
} AppleIIKeyboardType;

typedef enum
{
    APPLEIIKEYBOARD_STATE_NORMAL,
    APPLEIIKEYBOARD_STATE_RESET,
    APPLEIIKEYBOARD_STATE_ALTRESET,
    APPLEIIKEYBOARD_STATE_RESTART,
} AppleIIKeyboardState;

class AppleIIKeyboard : public OEComponent
{
public:
    AppleIIKeyboard();
    
	bool setValue(string name, string value);
	bool getValue(string name, string& value);
	bool setRef(string name, OEComponent *ref);
    bool init();
	void update();
    
    void notify(OEComponent *sender, int notification, void *data);
    
	OEChar read(OEAddress address);
	void write(OEAddress address, OEChar value);
	
protected:
    OEComponent *controlBus;
    OEComponent *floatingBus;
	OEComponent *monitor;
    
	AppleIIKeyboardType type;
    
    OEInt pressedKeypadKeyCount;
    
    OEChar keyLatch;
    bool keyStrobe;
    
    virtual void updateKeyFlags();
    virtual void sendKey(CanvasUnicodeChar key);
    void setKeyStrobe(bool value);
    virtual void setReset(bool value);
    virtual void setAltReset(bool value);
    
    void emptyPasteBuffer();
    
private:
    OEComponent *gamePort;
    
    AppleIIKeyboardState state;
    OEInt stateUsageId;
    
    queue<OEChar> pasteBuffer;
    
    void paste(wstring *s);
};

#endif

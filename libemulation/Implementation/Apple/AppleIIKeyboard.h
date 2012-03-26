
/**
 * libemulator
 * Apple II Keyboard
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II keyboard
 */

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
    
	OEUInt8 read(OEAddress address);
	void write(OEAddress address, OEUInt8 value);
	
private:
	AppleIIKeyboardType type;
    
    OEComponent *controlBus;
    OEComponent *floatingBus;
    OEComponent *gamePort;
	OEComponent *monitor;
    
    OEUInt8 keyLatch;
    AppleIIKeyboardState state;
    
    queue<OEUInt8> pasteBuffer;
    
    void updateShiftKeyMod();
    void sendKey(CanvasUnicodeChar key);
    void paste(wstring *s);
    void emptyPasteBuffer();
};

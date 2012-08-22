
/**
 * libemulation
 * Apple III Keyboard
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple III keyboard
 */

#ifndef _APPLEIIIKEYBOARD_H
#define _APPLEIIIKEYBOARD_H

#include "AppleIIKeyboard.h"

class AppleIIIKeyboard : public AppleIIKeyboard
{
public:
    AppleIIIKeyboard();
    
	OEChar read(OEAddress address);
	
private:
    OEChar appleIIIKeyFlags;
    
    void updateKeyFlags();
    void sendKey(CanvasUnicodeChar key);
    
    void setReset(bool value);
    void setAltReset(bool value);
};

#endif

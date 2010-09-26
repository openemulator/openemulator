
/**
 * libemulator
 * Apple II Keyboard
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II keyboard.
 */

#include "AppleIIKeyboard.h"

bool AppleIIKeyboard::setComponent(string name, OEComponent *component)
{
	if (name == "host")
		host = component;
	else
		return false;
	
	return true;
}

OEUInt8 AppleIIKeyboard::read(OEAddress address)
{
	return 0;
}

void AppleIIKeyboard::write(OEAddress address, OEUInt8 value)
{
}

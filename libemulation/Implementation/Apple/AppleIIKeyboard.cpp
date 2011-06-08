
/**
 * libemulator
 * Apple II Keyboard
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an Apple II keyboard
 */

#include "AppleIIKeyboard.h"

bool AppleIIKeyboard::setValue(string name, string value)
{
	if (name == "type")
		type = value;
	else
		return false;
	
	return true;
}

bool AppleIIKeyboard::getValue(string name, string& value)
{
	if (name == "type")
		value = type;
	else
		return false;
	
	return true;
}

bool AppleIIKeyboard::setRef(string name, OEComponent *ref)
{
	if (name == "monitor")
		monitor = ref;
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

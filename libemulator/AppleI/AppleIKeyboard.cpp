
/**
 * libemulator
 * Apple I Keyboard
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Keyboard
 */

#include "AppleIKeyboard.h"

#include "HostHID.h"
#include "MC6821.h"

#define APPLEIKEYBOARD_MASK	0x40

bool AppleIKeyboard::connect(const string &name, OEComponent *component)
{
	if (name == "hostHID")
	{
		hostHID = component;
		// component->addObserver(this);

	}
	else if (name == "pia")
		pia = component;
	else
		return false;
	
	return true;
}

void AppleIKeyboard::notify(int notification, OEComponent *component, void *data)
{
	OEHIDKeyEvent *event = (OEHIDKeyEvent *) data;
	
	if ((event->isDown) && (event->unicode < 128))
	{
		key = event->unicode;
		
		bool value = true;
		pia->ioctl(MC6821_SET_CA1, &value);
	}
}

OEUInt8 AppleIKeyboard::read(int address)
{
	bool value = false;
	pia->ioctl(MC6821_SET_CA1, &value);
	
	return key | APPLEIKEYBOARD_MASK;
}

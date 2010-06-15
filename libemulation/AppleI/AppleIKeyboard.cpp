
/**
 * libemulation
 * Apple I Keyboard
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Keyboard
 */

#include "AppleIKeyboard.h"

#include "Host.h"
#include "MC6821.h"

#define APPLEIKEYBOARD_MASK	0x40

AppleIKeyboard::AppleIKeyboard()
{
	host = NULL;
	pia = NULL;
}

bool AppleIKeyboard::connect(const string &name, OEComponent *component)
{
	if (name == "host")
	{
		if (host)
			host->removeObserver(this, HOST_HID_UNICODEKEYBOARD_EVENT);
		host = component;
		if (host)
			host->addObserver(this, HOST_HID_UNICODEKEYBOARD_EVENT);
	}
	else if (name == "pia")
		pia = component;
	else
		return false;
	
	return true;
}

void AppleIKeyboard::notify(int notification, OEComponent *component, void *data)
{
	HostHIDEvent *event = (HostHIDEvent *) data;
	
	key = event->usageId;
	
	bool value = true;
	pia->ioctl(MC6821_SET_CA1, &value);
}

OEUInt8 AppleIKeyboard::read(int address)
{
	bool value = false;
	pia->ioctl(MC6821_SET_CA1, &value);
	
	return key | APPLEIKEYBOARD_MASK;
}

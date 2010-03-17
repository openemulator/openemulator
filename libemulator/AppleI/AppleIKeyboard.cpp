
/**
 * libemulator
 * Apple I Keyboard
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Keyboard
 */

#include "AppleIKeyboard.h"
#include "MC6821.h"

#define APPLEIKEYBOARD_MASK	0x40

int AppleIKeyboard::ioctl(int message, void *data)
{
	switch (message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "hostKeyboard")
			{
				hostKeyboard = connection->component;
				hostKeyboard->addObserver(this);
			}
			break;
		}
		case OEIOCTL_NOTIFY:
		{
			OEIoctlNotification *notification = (OEIoctlNotification *) data;
			OEIoctlKeyEvent *keyEvent = (OEIoctlKeyEvent *) notification->data;
			
			if ((keyEvent->isDown) && (keyEvent->unicode < 128))
			{
				key = keyEvent->unicode;
			
				bool value = true;
				pia->ioctl(MC6821_SET_CA1, &value);
			}
		}
	}
	
	return false;
}

int AppleIKeyboard::read(int address)
{
	bool value = false;
	pia->ioctl(MC6821_SET_CA1, &value);
	
	return key | APPLEIKEYBOARD_MASK;
}

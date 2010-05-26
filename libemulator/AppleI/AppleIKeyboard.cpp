
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

int AppleIKeyboard::ioctl(int message, void *data)
{
	switch (message)
	{
		case OE_CONNECT:
		{
			OEConnection *connection = (OEConnection *) data;
			if (connection->name == "hostHID")
			{
				hostKeyboard = connection->component;
//				hostKeyboard->addObserver(this);
			}
			
			break;
		}
		case OE_NOTIFY:
		{
			OENotification *notification = (OENotification *) data;
			OEHIDEvent *event = (OEHIDEvent *) notification->data;
			
			if ((event->isDown) && (event->unicode < 128))
			{
				key = event->unicode;
				
				bool value = true;
				pia->ioctl(MC6821_SET_CA1, &value);
			}
			
			break;
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

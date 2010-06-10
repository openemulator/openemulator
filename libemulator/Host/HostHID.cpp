
/*
 *  HostHID.cpp
 *  OpenEmulator
 *
 *  Created by Marc S. Reßl on 25/05/10.
 *  Copyright 2010 ITBA. All rights reserved.
 *
 */

#include "HostHID.h"

HostHID::HostHID()
{
	updateLED = NULL;
}

bool HostHID::setProperty(const string &name, const string &value)
{
	if (name == "ledState")
	{
		ledState = getInt(value);
		if (observer)
			observer(ledState);
	}
	else
		return false;
	
	return true;
}

bool HostHID::getProperty(const string &name, string &value)
{
	if (name == "ledState")
		value = ledState;
	else
		return false;
	
	return true;
}

int HostHID::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTHID_REGISTER_HOST:
			updateLED = *((HostHIDUpdateLED *) data);
			updateLED(ledState);
			return true;
		case HOSTHID_UPDATE_LED:
		{
			HostHIDEvent *event = (HostHIDEvent *) data;
			int mask = (1 << (event->usageId));
			if (event->value)
				ledState |= mask;
			else
				ledState &= ~mask;
			observer(ledState);
			return true;
		}
	}
	
	return false;
}

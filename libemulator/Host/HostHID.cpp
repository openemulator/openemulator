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
	hostObserver.addObserver = NULL;
	hostObserver.removeObserver = NULL;
	hostObserver.notify = NULL;
}

bool HostHID::addObserver(OEComponent *component, int notification)
{
	hostObserver.addObserver(notification);
	return OEComponent::addObserver(component, notification);
}

bool HostHID::removeObserver(OEComponent *component, int notification)
{
	hostObserver.removeObserver(notification);
	return OEComponent::removeObserver(component, notification);
}

void HostHID::notify(int notification, OEComponent *component, void *data)
{
	hostObserver.notify(notification, data);
}

int HostHID::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTHID_REGISTER_HOST:
		{
			if (hostObserver.notify)
				removeObserver(this, HOSTHID_LED_EVENT);
			hostObserver = *((OEHostObserver *) data);
			if (hostObserver.notify)
				addObserver(this, HOSTHID_LED_EVENT);
			return true;
		}
	}
	
	return false;
}

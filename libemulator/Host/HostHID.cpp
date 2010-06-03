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

void HostHID::postNotification(int notification, void *data)
{
	hostObserver.notify(notification, data);
	OEComponent::postNotification(notification, data);
}

int HostHID::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTHID_REGISTER_HOST:
			OEHostObserver *observer = (OEHostObserver *) data;
			hostObserver = *observer;
			return true;
	}
	
	return false;
}

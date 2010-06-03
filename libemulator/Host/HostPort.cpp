
/**
 * libemulator
 * Host Port
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host ports (serial, parallel, midi, ethernet, usb, pdf)
 */

#include "HostPort.h"

HostPort::HostPort()
{
	hostObserver.addObserver = NULL;
	hostObserver.removeObserver = NULL;
	hostObserver.notify = NULL;
}

bool HostPort::addObserver(OEComponent *component, int notification)
{
	hostObserver.addObserver(notification);
	return OEComponent::addObserver(component, notification);
}

bool HostPort::removeObserver(OEComponent *component, int notification)
{
	hostObserver.removeObserver(notification);
	return OEComponent::removeObserver(component, notification);
}

void HostPort::postNotification(int notification, void *data)
{
	hostObserver.notify(notification, data);
	OEComponent::postNotification(notification, data);
}

int HostPort::ioctl(int message, void *data)
{
	switch (message)
	{
		case HostPort_REGISTER_HOST:
			OEHostObserver *observer = (OEHostObserver *) data;
			hostObserver = *observer;
			return true;
	}
	
	return false;
}

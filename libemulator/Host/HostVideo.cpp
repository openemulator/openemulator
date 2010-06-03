
/**
 * libemulator
 * Host Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host video events
 */

#include "HostVideo.h"

// Calculation of contents
//
// xn = ceil(sqrt(N))
// un = ceil(N/xn)
//
// The size of each screen is:
// maxWidth * xn
// maxHeight * yn

HostVideo::HostVideo()
{
	hostObserver.addObserver = NULL;
	hostObserver.removeObserver = NULL;
	hostObserver.notify = NULL;
}

bool HostVideo::setProperty(const string &name, const string &value)
{
	if (name == "window")
		window = value;
	else
		return false;
	
	return true;
}

bool HostVideo::getProperty(const string &name, string &value)
{
	if (name == "window")
		value = window;
	else
		return false;
	
	return true;
}

bool HostVideo::addObserver(OEComponent *component, int notification)
{
	hostObserver.addObserver(notification);
	return OEComponent::addObserver(component, notification);
}

bool HostVideo::removeObserver(OEComponent *component, int notification)
{
	hostObserver.removeObserver(notification);
	return OEComponent::removeObserver(component, notification);
}

void HostVideo::postNotification(int notification, void *data)
{
	hostObserver.notify(notification, data);
	OEComponent::postNotification(notification, data);
}

int HostVideo::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTVIDEO_REGISTER_HOST:
			OEHostObserver *observer = (OEHostObserver *) data;
			hostObserver = *observer;
			return true;
		case HOSTVIDEO_ADD_SCREEN:
			return true;
		case HOSTVIDEO_REMOVE_SCREEN:
			return true;
	}
	
	return false;
}

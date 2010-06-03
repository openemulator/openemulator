
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
	if (hostObserver.addObserver)
		hostObserver.addObserver(notification);
	return OEComponent::addObserver(component, notification);
}

bool HostVideo::removeObserver(OEComponent *component, int notification)
{
	if (hostObserver.removeObserver)
		hostObserver.removeObserver(notification);
	return OEComponent::removeObserver(component, notification);
}

void HostVideo::notify(int notification, OEComponent *component, void *data)
{
	hostObserver.notify(notification, data);
}

int HostVideo::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTVIDEO_REGISTER_HOST:
		{
			if (hostObserver.notify)
				removeObserver(this, HOSTVIDEO_FRAME_DID_RENDER);
			hostObserver = *((OEHostObserver *) data);
			if (hostObserver.notify)
				addObserver(this, HOSTVIDEO_FRAME_DID_RENDER);
			return true;
		}
		case HOSTVIDEO_ADD_SCREEN:
			videoFrames.push_back((HostVideoFrame *) data);
			return true;
		case HOSTVIDEO_REMOVE_SCREEN:
			return true;
	}
	
	return false;
}

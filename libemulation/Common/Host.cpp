
/**
 * libemulation
 * Host
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host events
 */

#include "Host.h"

Host::Host()
{
	hostObserver = NULL;
	
	hidLEDState = 0;
}

bool Host::setProperty(const string &name, const string &value)
{
	if (name == "notes")
		notes = value;
	else if (name == "powerState")
	{
		int state = getInt(value);
		if (powerState != state)
		{
			powerState = state;
		}
	}
	else if (name == "hidMouseCapture")
		hidMouseCapture = getInt(value);
	else if (name == "videoWindow")
		videoWindow = value;
	else
		return false;
	
	return true;
}

bool Host::getProperty(const string &name, string &value)
{
	if (name == "notes")
		value = notes;
	else if (name == "powerState")
		value = powerState;
	else if (name == "hidMouseCapture")
		value = hidMouseCapture;
	else if (name == "videoWindow")
		value = videoWindow;
	else
		return false;
	
	return true;
}

void Host::notify(int notification, OEComponent *component, void *data)
{
	if (hostObserver)
		hostObserver(notification, data);
}

int Host::ioctl(int message, void *data)
{
	switch(message)
	{
		case HOST_REGISTER_HOST:
			if (data)
				hostObserver = (HostObserver) data;
			return true;
		case HOST_ADD_SCREEN:
			return true;
		case HOST_REMOVE_SCREEN:
			return true;
		case HOST_UPDATE_VIDEO:
			postNotification(HOST_VIDEO_DID_UPDATE, NULL);
			return true;
	}
	
	return false;
}

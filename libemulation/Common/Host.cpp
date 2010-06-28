
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
	hostPower = NULL;
	hostVideo = NULL;
	hostHID = NULL;
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
			if (hostPower)
				hostPower(emulation, NULL);
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
		value = getString(powerState);
	else if (name == "hidMouseCapture")
		value = getString(hidMouseCapture);
	else if (name == "videoWindow")
		value = videoWindow;
	else
		return false;
	
	return true;
}

bool Host::addScreen(HostVideoScreen *screen)
{
	videoScreens.push_back(screen);
	
	return true;
}

bool Host::removeScreen(HostVideoScreen *screen)
{
	HostVideoScreens::iterator first = videoScreens.begin();
	HostVideoScreens::iterator last = videoScreens.end();
	HostVideoScreens::iterator i = remove(first, last, screen);
	
	if (i != last)
		videoScreens.erase(i, last);
	
	return (i != last);
}

int Host::ioctl(int message, void *data)
{
	bool status = false;
	
	switch(message)
	{
		case HOST_REGISTER_EMULATION:
			emulation = data;
			return true;
			
		case HOST_REGISTER_POWER:
			hostPower = (HostObserver) data;
			return true;
			
		case HOST_REGISTER_VIDEO:
			hostVideo = (HostObserver) data;
			return true;
			
		case HOST_REGISTER_HID:
			hostHID = (HostObserver) data;
			return true;
			
		case HOST_ADD_SCREEN:
			status = addScreen((HostVideoScreen *) data);
			if (hostVideo)
				hostVideo(emulation, &videoScreens);
			return status;
			
		case HOST_REMOVE_SCREEN:
			status = removeScreen((HostVideoScreen *) data);
			if (hostVideo)
				hostVideo(emulation, &videoScreens);
			return status;
			
		case HOST_UPDATE_VIDEO:
			if (hostVideo)
				hostVideo(emulation, &videoScreens);
			return true;
			
		case HOST_SET_KEYBOARD_LED:
			if (hostHID)
				hostHID(emulation, data);
			return true;
	}
	
	return false;
}

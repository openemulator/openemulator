
/**
 * libemulation
 * Host
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host events
 */

#include "Host.h"

bool Host::setProperty(const string &name, const string &value)
{
	if (name == "notes")
		notes = value;
	else if (name == "powerState")
		powerState = getInt(value);
	else if (name == "hidMouseCapture")
		hidMouseCapture = getInt(value);
	else if (name == "hidKeyboardLEDs")
		hidKeyboardLEDs = getInt(value);
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
	else if (name == "hidKeyboardLEDs")
		value = getString(hidKeyboardLEDs);
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
		case HOST_ADD_SCREEN:
			status = addScreen((HostVideoScreen *) data);
			return status;
			
		case HOST_REMOVE_SCREEN:
			status = removeScreen((HostVideoScreen *) data);
			return status;
			
		case HOST_GET_SCREENS:
		{
			HostVideoScreens **videoScreensP = (HostVideoScreens **) data;
			*videoScreensP = &videoScreens;
			return true;
		}
		
		case HOST_IS_COPYABLE:
			return (observerMap[HOST_CLIPBOARD_COPY_EVENT].size() != 0);
			
		case HOST_IS_PASTEABLE:
			return (observerMap[HOST_CLIPBOARD_PASTE_EVENT].size() != 0);
	}
	
	return false;
}


/**
 * libemulator
 * Host
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host events
 */

#include "Host.h"

Host::Host()
{
	powerState = 0;
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
	else if (name == "videoWindow")
		value = videoWindow;
	else
		return false;
	
	return true;
}

int Host::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOST_SET_POWERSTATE:
			powerState = *((int *) data);
			return true;
	}
	
	return false;
}

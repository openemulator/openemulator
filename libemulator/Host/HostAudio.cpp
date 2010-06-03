
/**
 * libemulator
 * Host Audio
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host audio events
 */

#include "HostAudio.h"

bool HostAudio::setProperty(const string &name, const string &value)
{
	if (name == "state")
	{
		int newState = getInt(value);
		if (state != newState)
		{
			state = newState;
			postNotification(HOSTAUDIO_SYSTEM_STATE_CHANGED, &state);
		}
	}
	else if (name == "runTime")
		runTime = getFloat(value);
	else if (name == "notes")
		notes = value;
	else
		return false;
	
	return true;
}

bool HostAudio::getProperty(const string &name, string &value)
{
	if (name == "state")
		value = state;
	else if (name == "runTime")
		value = runTime;
	else if (name == "notes")
		value = notes;
	else
		return false;
	
	return true;
}

int HostAudio::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTAUDIO_ADD_RUNTIME:
		{
			runTime += *((double *) data);
			return true;
		}
	}
	
	return false;
}


/**
 * libemulator
 * Host Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host video events
 */

#include "HostVideo.h"

bool HostVideo::setProperty(string name, string value)
{
	if (name == "window")
		;
	else
		return false;
	
	return true;
}

bool HostVideo::getProperty(string name, string &value)
{
	if (name == "window")
		;
	else
		return false;
	
	return true;
}


int HostVideo::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTVIDEO_ADD_SCREEN:
			return true;
		case HOSTVIDEO_REMOVE_SCREEN:
			return true;
	}
	
	return false;
}

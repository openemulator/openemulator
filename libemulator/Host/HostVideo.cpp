
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

bool HostVideo::setProperty(const string &name, const string &value)
{
	if (name == "window")
		;
	else
		return false;
	
	return true;
}

bool HostVideo::getProperty(const string &name, string &value)
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
		case HOSTVIDEO_QUERY_FRAME:
			return true;
		case HOSTVIDEO_RETURN_FRAME:
			return true;
		case HOSTVIDEO_SET_CALLBACK:
			return true;
	}
	
	return false;
}

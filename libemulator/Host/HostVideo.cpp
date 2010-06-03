
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
	callbacks.postFrame = NULL;
	callbacks.openDevice = NULL;
	callbacks.closeDevice = NULL;
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

int HostVideo::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTVIDEO_REGISTER_HOST:
			callbacks = *((HostVideoCallbacks *) data);
			return true;
		case HOSTVIDEO_ADD_SCREEN:
			frames.push_back((HostVideoFrame *) data);
			return true;
		case HOSTVIDEO_REMOVE_SCREEN:
			for (HostVideoFrames::iterator i = frames.begin();
				 i != frames.end();
				 i++)
			{
				if ((*i) == data)
					frames.erase(i);
			}
			return true;
	}
	
	return false;
}

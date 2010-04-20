
/**
 * libemulator
 * Host Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host video events
 */

#include "HostVideo.h"

int HostVideo::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOST_VIDEO_ADD_SCREEN:
			return true;
		case HOST_VIDEO_REMOVE_SCREEN:
			return true;
		case HOST_VIDEO_GET_SCREEN_NUM:
			return true;
		case HOST_VIDEO_GET_SCREEN:
			return true;
	}
	
	return false;
}

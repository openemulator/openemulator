
/**
 * libemulator
 * Apple I Video
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls the Apple I Video
 */

#include "AppleIVideo.h"

int AppleIVideo::ioctl(int message, void *data)
{
	switch(message)
	{
		case OEIOCTL_CONNECT:
		{
			OEIoctlConnection *connection = (OEIoctlConnection *) data;
			if (connection->name == "system")
			{
				OEComponent *system = connection->component;
				system->addObserver(this);
			}
			break;
		}
		case OEIOCTL_SET_RESOURCE:
		{
			OEIoctlData *resource = (OEIoctlData *) data;
			if (resource->name == "image")
				characterSet = resource->data;
			break;
		}
	}
	
	return false;
}

void AppleIVideo::write(int address, int value)
{
	
	
}

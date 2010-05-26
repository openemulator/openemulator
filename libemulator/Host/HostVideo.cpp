
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
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
//			if (property->name == "window")
//				runTime = getFloat(property->value);
			
			break;
		}
		case OE_GET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
//			if (property->name == "window")
//				property->value = runTime;
			
			break;
		}
		case HOSTVIDEO_ADD_SCREEN:
			return true;
		case HOSTVIDEO_REMOVE_SCREEN:
			return true;
	}
	
	return false;
}

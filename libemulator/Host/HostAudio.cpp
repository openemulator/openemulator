
/**
 * libemulator
 * Host Audio
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host audio events
 */

#include <math.h>

#include "HostAudio.h"

int HostAudio::ioctl(int message, void *data)
{
	switch (message)
	{
		case OE_SET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "runTime")
				runTime = getFloat(property->value);
			else if (property->name == "isPaused")
				isPaused = getInt(property->value);
			
			break;
		}
		case OE_GET_PROPERTY:
		{
			OEProperty *property = (OEProperty *) data;
			if (property->name == "runTime")
				property->value = runTime;
			else if (property->name == "isPaused")
				property->value = isPaused;
			
			break;
		}
		case HOSTAUDIO_RENDERBUFFER:
		{
			postNotification(HOSTAUDIO_RENDER_WILL_START,
							 renderWillStartObservers,
							 data);
			postNotification(HOSTAUDIO_RENDER_DID_START,
							 renderWillStartObservers,
							 data);
			postNotification(HOSTAUDIO_RENDER_WILL_END, 
							 renderWillStartObservers,
							 data);
			postNotification(HOSTAUDIO_RENDER_DID_END, 
							 renderWillStartObservers,
							 data);
			
			return true;
		}
		case HOSTAUDIO_ADD_RUNTIME:
		{
			runTime += *((double *) data);
			break;
		}
		case HOSTAUDIO_GET_RUNTIME:
		{
			*((double *) data) = runTime;
			break;
		}
	}
	
	return false;
}

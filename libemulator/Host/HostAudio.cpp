
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

HostAudio::HostAudio()
{
	f = rand() * (1000.0 / 32767.0);
}

int HostAudio::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTAUDIO_RENDERBUFFER:
		{
			postNotification(HOSTAUDIO_RENDER_WILL_START, data);
			postNotification(HOSTAUDIO_RENDER_DID_START, data);
			postNotification(HOSTAUDIO_RENDER_WILL_END, data);
			postNotification(HOSTAUDIO_RENDER_DID_END, data);
			
			return true;
		}
	}
	
	return false;
}

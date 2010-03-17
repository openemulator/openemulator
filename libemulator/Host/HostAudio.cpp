
/**
 * libemulator
 * Host Audio
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host audio events
 */

#include "HostAudio.h"

int HostAudio::ioctl(int message, void *data)
{
	switch (message)
	{
		case HOSTAUDIO_RENDERBUFFER:
			postNotification(HOSTAUDIO_BUFFERSTART, data);
			postNotification(HOSTAUDIO_BUFFERRENDER, data);
			postNotification(HOSTAUDIO_BUFFEREND, data);
			return true;
		case HOSTAUDIO_GETBUFFER:
			return true;
	}
	
	return false;
}

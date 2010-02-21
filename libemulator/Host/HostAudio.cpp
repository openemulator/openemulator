
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
		case HOST_AUDIO_RENDERBUFFER:
			postNotification(HOST_AUDIO_BUFFERSTART, NULL);
			postNotification(HOST_AUDIO_BUFFEREND, NULL);
			return true;
		case HOST_AUDIO_GETBUFFER:
			return true;
	}
	
	return false;
}


/**
 * libemulator
 * Host Audio
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls host audio events
 */

#include "OEComponent.h"

// Messages
enum
{
	HOSTAUDIO_RENDERBUFFER = OEIOCTL_USER,
	HOSTAUDIO_GETBUFFER,
	HOSTAUDIO_SET_PAUSE,
	HOSTAUDIO_GET_PAUSE,
	HOSTAUDIO_GET_RUNTIME,
};

// Notifications
enum
{
	HOSTAUDIO_BUFFERSTART,
	HOSTAUDIO_BUFFERRENDER,
	HOSTAUDIO_BUFFEREND,
};

// Types
typedef struct
{
	float *inputData;
	float *outputData;
	int sampleRate;
	int frameNum;
	int channelNum;
} HostAudioBuffer;

class HostAudio : public OEComponent
{
public:
	int ioctl(int message, void *data);
};

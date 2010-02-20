
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
	HOST_AUDIO_RENDERBUFFER,
	HOST_AUDIO_GETBUFFER,
	HOST_AUDIO_SET_PAUSE,
	HOST_AUDIO_GET_PAUSE,
	HOST_AUDIO_GET_RUNTIME,
};

// Notifications
enum
{
	HOST_AUDIO_BUFFERSTART,
	HOST_AUDIO_BUFFEREND,
};

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

};

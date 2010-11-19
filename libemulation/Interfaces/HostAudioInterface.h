
/**
 * libemulation
 * Host audio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the host audio interface
 */

#ifndef _HOSTAUDIOINTERFACE_H
#define _HOSTAUDIOINTERFACE_H

typedef enum
{
	HOST_AUDIO_FRAME_WILL_BEGIN,
	HOST_AUDIO_FRAME_WILL_RENDER,
	HOST_AUDIO_FRAME_WILL_END,
} HostAudioNotifications;

typedef struct
{
	float sampleRate;
	int channelNum;
	int frameNum;
	const float *input;
	float *output;
} HostAudioBuffer;

#endif

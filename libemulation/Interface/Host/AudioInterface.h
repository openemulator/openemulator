
/**
 * libemulation
 * Audio interface
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Defines the Audio interface
 */

#ifndef _AUDIOINTERFACE_H
#define _AUDIOINTERFACE_H

// Notifications send AudioBuffer

typedef enum
{
	AUDIO_FRAME_WILL_RENDER,
	AUDIO_FRAME_IS_RENDERING,
	AUDIO_FRAME_DID_RENDER,
} AudioNotification;

typedef struct
{
	float sampleRate;
	int channelNum;
	int frameNum;
	const float *input;
	float *output;
} AudioBuffer;

#endif

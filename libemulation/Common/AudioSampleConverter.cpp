
/**
 * libemulation
 * Audio sample converter
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an audio sample converter (8 and 16 bit)
 */

#include <math.h>

#include "AudioSampleConverter.h"
#include "HostAudioInterface.h"

AudioSampleConverter::AudioSampleConverter()
{
	hostAudio = NULL;
	controlBus = NULL;
}

bool AudioSampleConverter::setRef(string name, OEComponent *id)
{
	if (name == "hostAudio")
	{
		replaceObserver(hostAudio, id, HOST_AUDIO_FRAME_WILL_RENDER);
		hostAudio = id;
	}
	else if (name == "controlBus")
		controlBus = id;
	else
		return false;
	
	return true;
}

void AudioSampleConverter::notify(OEComponent *sender, int notification, void *data)
{
	switch(notification)
	{
		case HOST_AUDIO_FRAME_WILL_RENDER:
		{
			HostAudioBuffer *buffer = (HostAudioBuffer *) data;
			float *out = buffer->output;
			
			for(int i = 0; i < buffer->frameNum; i++)
			{
				float x = 0.1 * sin(phase);
				phase += 2 * M_PI * 500.0 / buffer->sampleRate;
				
				for (int ch = 0; ch < buffer->channelNum; ch++)
					*out++ += x;
			}
			
			// To-Do: Implement simulation
			break;
		}
	}
}

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
#include "AudioInterface.h"

AudioSampleConverter::AudioSampleConverter()
{
	audio = NULL;
	controlBus = NULL;
}

bool AudioSampleConverter::setRef(string name, OEComponent *ref)
{
	if (name == "audio")
	{
		removeObserver(audio, AUDIO_FRAME_WILL_RENDER);
		removeObserver(audio, AUDIO_FRAME_DID_RENDER);
		audio = ref;
		addObserver(audio, AUDIO_FRAME_WILL_RENDER);
		addObserver(audio, AUDIO_FRAME_DID_RENDER);
	}
	else if (name == "controlBus")
		controlBus = ref;
	else
		return false;
	
	return true;
}

void AudioSampleConverter::notify(OEComponent *sender, int notification, void *data)
{
	// To-Do: Implement simulation
}

OEUInt8 AudioSampleConverter::read(OEAddress address)
{
	return 0;
}

void AudioSampleConverter::write(OEAddress address, OEUInt8 value)
{
}

OEUInt16 AudioSampleConverter::read16(OEAddress address)
{
	return 0;
}

void AudioSampleConverter::write16(OEAddress address, OEUInt16 value)
{
}

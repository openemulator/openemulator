
/**
 * libemulation
 * ADC/DAC
 * (C) 2010 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Controls an audio ADC/DAC (8 and 16 bit)
 */

#include "ADCDAC.h"
#include "ControlBus.h"

ADCDAC::ADCDAC()
{
	audio = NULL;
	controlBus = NULL;
    
    audioBuffer = NULL;
}

bool ADCDAC::setRef(string name, OEComponent *ref)
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

void ADCDAC::notify(OEComponent *sender, int notification, void *data)
{
	// To-Do: Implement simulation
    audioBuffer = (AudioBuffer *)data;
}

OEUInt8 ADCDAC::read(OEAddress address)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
	return 0;
}

void ADCDAC::write(OEAddress address, OEUInt8 value)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
}

OEUInt16 ADCDAC::read16(OEAddress address)
{
	return 0;
}

void ADCDAC::write16(OEAddress address, OEUInt16 value)
{
}

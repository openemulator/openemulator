
/**
 * libemulation
 * Audio codec
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an 8/16 bit audio codec (ADC/DAC)
 */

#include "AudioCodec.h"
#include "ControlBus.h"

AudioCodec::AudioCodec()
{
    audio = NULL;
    controlBus = NULL;
    
    audioBuffer = NULL;
}

bool AudioCodec::setRef(string name, OEComponent *ref)
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

void AudioCodec::notify(OEComponent *sender, int notification, void *data)
{
    // To-Do: Implement simulation
    audioBuffer = (AudioBuffer *)data;
}

OEUInt8 AudioCodec::read(OEAddress address)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    return 0;
}

void AudioCodec::write(OEAddress address, OEUInt8 value)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
}

OEUInt16 AudioCodec::read16(OEAddress address)
{
    return 0;
}

void AudioCodec::write16(OEAddress address, OEUInt16 value)
{
}

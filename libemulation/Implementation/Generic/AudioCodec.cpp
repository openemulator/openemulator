
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
        if (audio)
        {
            audio->removeObserver(this, AUDIO_FRAME_WILL_RENDER);
            audio->removeObserver(this, AUDIO_FRAME_DID_RENDER);
        }
        audio = ref;
        if (audio)
        {
            audio->addObserver(this, AUDIO_FRAME_WILL_RENDER);
            audio->addObserver(this, AUDIO_FRAME_DID_RENDER);
        }
    }
    else if (name == "controlBus")
        controlBus = ref;
    else
        return false;
    
    return true;
}

bool AudioCodec::init()
{
    if (!audio)
    {
		logMessage("audio not connected");
        return false;
    }
    
    if (!controlBus)
    {
		logMessage("controlBus not connected");
        return false;
    }
    
    return true;
}

void AudioCodec::notify(OEComponent *sender, int notification, void *data)
{
    if (notification == AUDIO_FRAME_WILL_RENDER)
        audioBuffer = (AudioBuffer *)data;
    else if (notification == AUDIO_FRAME_DID_RENDER)
    {
        // To-Do: transform rest of frame
    }
}

OEUInt8 AudioCodec::read(OEAddress address)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    return 128 + (OEUInt8)(audioBuffer->input[audioBuffer->channelNum *
                                              ((OEUInt32) audioBufferIndex)] * 127.0F);
}

void AudioCodec::write(OEAddress address, OEUInt8 value)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
}

OEUInt16 AudioCodec::read16(OEAddress address)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    return (OEInt16)(audioBuffer->input[audioBuffer->channelNum *
                                        ((OEUInt32) audioBufferIndex)] * 32767.0F);
}

void AudioCodec::write16(OEAddress address, OEUInt16 value)
{
}

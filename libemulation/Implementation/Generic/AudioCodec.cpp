
/**
 * libemulation
 * Audio codec
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an 8/16 bit audio codec (ADC/DAC)
 */

#include "AudioCodec.h"
#include "ControlBusInterface.h"

AudioCodec::AudioCodec()
{
    audio = NULL;
    controlBus = NULL;
    
    highpassFrequency = 8000;
    lowpassFrequency = 20;
    timeAccuracy = 0.000001;
    sidelobes = 6;
    
    audioBuffer = NULL;
    
    bufferFrameNum = 0;
}

bool AudioCodec::setValue(string name, string value)
{
    if (name == "highpassFrequency")
        highpassFrequency = getFloat(value);
    else if (name == "lowpassFrequency")
        lowpassFrequency = getFloat(value);
    else if (name == "timeAccuracy")
        timeAccuracy = getFloat(value);
    else if (name == "sidelobes")
        sidelobes = (OEUInt32) getUInt(value);
    else
        return false;
    
    return true;
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

void AudioCodec::update()
{
    updateSynth();
}

void AudioCodec::notify(OEComponent *sender, int notification, void *data)
{
    if (notification == AUDIO_FRAME_WILL_RENDER)
    {
        audioBuffer = (AudioBuffer *)data;
        
        if (audioBuffer->frameNum != bufferFrameNum)
            updateSynth();
    }
    else if (notification == AUDIO_FRAME_DID_RENDER)
        renderBuffer();
}

OEUInt8 AudioCodec::read(OEAddress address)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    OEUInt32 index = audioBuffer->channelNum * ((OEUInt32) audioBufferIndex);
    index += address % audioBuffer->channelNum;
    
    return 128 + (OEUInt8)(audioBuffer->input[index] * 127.0F);
}

void AudioCodec::write(OEAddress address, OEUInt8 value)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    setSample(audioBufferIndex, (OEUInt32) address % audioBuffer->channelNum, (value - 128) / 128.0F);
}

OEUInt16 AudioCodec::read16(OEAddress address)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    OEUInt32 index = audioBuffer->channelNum * ((OEUInt32) audioBufferIndex);
    index += address % audioBuffer->channelNum;
    
    return (OEInt16)(audioBuffer->input[index] * 32767.0F);
}

void AudioCodec::write16(OEAddress address, OEUInt16 value)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    setSample(audioBufferIndex, (OEUInt32) address % audioBuffer->channelNum, ((OEInt16) value) / 32768.0F);
}

void AudioCodec::updateSynth()
{
    bufferFrameNum = audioBuffer->frameNum;
    
    // Update tables
}

void AudioCodec::setSample(float index, int channel, float level)
{
    // Add impulse
}

void AudioCodec::renderBuffer()
{
    // Integrate
    // Lowpass filter
}

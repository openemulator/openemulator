
/**
 * libemulation
 * Audio codec
 * (C) 2010-2011 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements an 8/16 bit audio codec (ADC/DAC)
 */

#include <math.h>

#include "AudioCodec.h"

#include "ControlBusInterface.h"

// Notes:
// * This audio codec uses bandwidth-limited impulses (BLIT) to bandwidth
//   limit the input, and to produce high-quality output waveforms
// * timeAccuracy ensures that impulse generation achieves a certain level
//   of time accuracy
// * filterSize is the minimum size of the filter that is used for
//   limiting bandwidth
// * lowFrequency is the lower frequency of the accepted band
// * highFrequency is the higher frequency of the accepted band
// * A low timing accuracy will result in timing errors and background noise.
//   It is best to adapt this value to the time resolution of the generator
// * The analysis and synthesis will always leak beyond the higher frequency.
//   The lower the high frequency and the larger the filter size is,
//   less likely leaks will occur
//
// Reference:
// * https://ccrma.stanford.edu/~stilti/papers/blit.pdf

AudioCodec::AudioCodec()
{
    timeAccuracy = 0.000001;
    filterSize = 17;
    lowFrequency = 20;
    highFrequency = 20000;
    
    audio = NULL;
    controlBus = NULL;
    
    audioBuffer = NULL;
    
    sampleRate = 0;
    channelNum = 0;
    frameNum = 0;
}

bool AudioCodec::setValue(string name, string value)
{
    if (name == "timeAccuracy")
        timeAccuracy = getFloat(value);
    else if (name == "filterSize")
        filterSize = getOEInt(value);
    else if (name == "lowFrequency")
        lowFrequency = getFloat(value);
    else if (name == "highFrequency")
        highFrequency = getFloat(value);
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
            audio->removeObserver(this, AUDIO_BUFFER_WILL_RENDER);
            audio->removeObserver(this, AUDIO_BUFFER_DID_RENDER);
        }
        audio = ref;
        if (audio)
        {
            audio->addObserver(this, AUDIO_BUFFER_WILL_RENDER);
            audio->addObserver(this, AUDIO_BUFFER_DID_RENDER);
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
    
    updateSynth();
    
    return true;
}

void AudioCodec::update()
{
    updateSynth();
}

void AudioCodec::notify(OEComponent *sender, int notification, void *data)
{
    if (notification == AUDIO_BUFFER_WILL_RENDER)
    {
        audioBuffer = (AudioBuffer *)data;
        
        if ((audioBuffer->sampleRate != sampleRate) ||
            (audioBuffer->frameNum != frameNum) ||
            (audioBuffer->channelNum != channelNum))
        {
            sampleRate = audioBuffer->sampleRate;
            frameNum = audioBuffer->frameNum;
            channelNum = audioBuffer->channelNum;
            sampleNum = frameNum * channelNum;
            
            updateSynth();
            
            lastInput.clear();
            lastInput.resize(channelNum);
            buffer.clear();
            buffer.resize(2 * sampleNum);
            lastOutput.clear();
            lastOutput.resize(channelNum);
        }
        
        memcpy(&buffer.front(), &buffer.front() + sampleNum, sampleNum * sizeof(float));
        memset(&buffer.front() + sampleNum, 0, sampleNum * sizeof(float));
    }
    else if (notification == AUDIO_BUFFER_DID_RENDER)
        synthBuffer();
}

OEChar AudioCodec::read(OEAddress address)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    OEInt index = audioBuffer->channelNum * ((OEInt) audioBufferIndex);
    index += address % audioBuffer->channelNum;
    
    return 128 + (OEChar)(audioBuffer->input[index] * 127.0F);
}

void AudioCodec::write(OEAddress address, OEChar value)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    if (address < audioBuffer->channelNum)
        setSynth(audioBufferIndex, (OEInt) address, (value - 128) / 128.0F);
}

OEShort AudioCodec::read16(OEAddress address)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    OEInt index = audioBuffer->channelNum * ((OEInt) audioBufferIndex);
    index += address % audioBuffer->channelNum;
    
    return (OESShort)(audioBuffer->input[index] * 32767.0F);
}

void AudioCodec::write16(OEAddress address, OEShort value)
{
    float audioBufferIndex;
    
    controlBus->postMessage(this, CONTROLBUS_GET_AUDIOBUFFERINDEX, &audioBufferIndex);
    
    if (address < audioBuffer->channelNum)
        setSynth(audioBufferIndex, (OEInt) address, ((OESShort) value) / 32768.0F);
}

void AudioCodec::updateSynth()
{
    if (!audioBuffer)
        return;
    
    integrationAlpha = 1.0 / (1.0 + lowFrequency / audioBuffer->sampleRate);
    
    float sincCutoff = 2.0 * highFrequency / audioBuffer->sampleRate;
    if (sincCutoff >= 0.9)
        sincCutoff = 0.9;
    
    // Produce an odd-sized filter
    impulseFilterHalfSize = (filterSize / 2);
    impulseFilterSize = impulseFilterHalfSize * 2 + 1;
    
    // Calculate number of impulses
    impulseTableEntryNum = (OEInt) (1.0 / (timeAccuracy * audioBuffer->sampleRate));
    impulseTableEntrySize = (OEInt) getNextPowerOf2(impulseFilterSize);
    impulseTable.resize(impulseTableEntryNum * impulseTableEntrySize);
    
    for (OEInt phase = 0; phase < impulseTableEntryNum; phase++)
    {
        float *impulseEntry = &impulseTable.front() + phase * impulseTableEntrySize;
        
        float energy = 0;
        
        for (OEInt n = 0; n < impulseFilterSize; n++)
        {
            float i = n - (float) impulseFilterHalfSize;
            float x = i - (float) phase / impulseTableEntryNum;
            
            // Sinc
            x *= sincCutoff;
            x = (x == 0.0F) ? 1.0F : x = sinf(M_PI * x) / (M_PI * x);
            
            // Apply hamming window
            x *= 0.54 + 0.46 * cosf(M_PI * i / impulseFilterHalfSize);
            
            // Calculate gain
            energy += x;
            
            impulseEntry[n] = x;
        }
        
        float gain = 1.0 / energy;
        
        // Normalize
        for (OEInt n = 0; n < impulseFilterSize; n++)
            impulseEntry[n] *= gain;
    }
}

void AudioCodec::setSynth(float index, OEInt channel, float level)
{
    float gain = level - lastInput[channel];
    lastInput[channel] = level;
    
    OEInt n = index;
    float nr = index - n;
    
    OEInt phase = (OEInt) (nr * impulseTableEntryNum);
    
    float *x = &impulseTable.front() + phase * impulseTableEntrySize;
    float *y = &buffer.front() + n * channelNum + channel;
    
    for (OEInt i = 0; i < impulseFilterSize; i++, y += channelNum)
        *y += gain * x[i];
}

void AudioCodec::synthBuffer()
{
    for (OEInt ch = 0; ch < channelNum; ch++)
    {
        float *x = &buffer.front() + ch;
        float *y = audioBuffer->output + ch;
        
        float yLast = lastOutput[ch];
        
        for (OEInt i = 0; i < sampleNum; i += channelNum)
        {
            yLast = integrationAlpha * yLast + x[i];
            y[i] += yLast;
        }
        
        lastOutput[ch] = yLast;
    }
}

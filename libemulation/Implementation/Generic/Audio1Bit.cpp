
/**
 * libemulation
 * Audio Codec 1-bit
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 1-bit Audio Codec
 */

#include "Audio1Bit.h"

#include "AudioInterface.h"

Audio1Bit::Audio1Bit()
{
    audioCodec = NULL;
    
    noiseRejection = 0.04;
    volume = 1;
    
    inputThreshold = 0;
    
    outputState = false;
}

bool Audio1Bit::setValue(string name, string value)
{
    if (name == "noiseRejection")
        noiseRejection = getFloat(value);
    else if (name == "volume")
        volume = getFloat(value);
    else
        return false;
    
    return true;
}

bool Audio1Bit::getValue(string name, string& value)
{
    if (name == "noiseRejection")
        value = getString(noiseRejection);
    else if (name == "volume")
        value = getString(volume);
    else
        return false;
    
    return true;
}

bool Audio1Bit::setRef(string name, OEComponent *ref)
{
    if (name == "audioCodec")
        audioCodec = ref;
    else
        return false;
    
    return true;
}

bool Audio1Bit::init()
{
    if (!audioCodec)
    {
        logMessage("audioCodec not connected");
        
        return false;
    }
    
    return true;
}

void Audio1Bit::update()
{
    inputLevel = 32768 * inputThreshold;
    
    outputLevel = 16384 * getLevelFromVolume(volume);
}

bool Audio1Bit::readAudioInput()
{
    // Schmitt trigger
    if (audioCodec->read16(0) >= inputThreshold)
    {
        inputThreshold = -inputLevel;
        
        return true;
    }
    else
    {
        inputThreshold = noiseRejection;
        
        return false;
    }
}

void Audio1Bit::toggleAudioOutput()
{
    outputState = !outputState;
    
    audioCodec->write16(0, outputLevel * outputState);
    audioCodec->write16(1, outputLevel * outputState);
}

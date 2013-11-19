
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
    
    noiseRejection = 0.04F;
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
    OECheckComponent(audioCodec);
    
    update();
    
    return true;
}

void Audio1Bit::update()
{
    inputLevel = 32768 * noiseRejection;
    
    outputLevel = 16384 * getLevelFromVolume(volume);
}

bool Audio1Bit::readAudioInput()
{
    OESShort value = audioCodec->read16(0);
    
    // Schmitt trigger
    if (value >= inputThreshold)
    {
        inputThreshold = -inputLevel;
        
        return true;
    }
    else
    {
        inputThreshold = inputLevel;
        
        return false;
    }
}

void Audio1Bit::toggleAudioOutput()
{
    outputState = !outputState;
    
    audioCodec->write16(0, outputLevel * outputState);
    audioCodec->write16(1, outputLevel * outputState);
}

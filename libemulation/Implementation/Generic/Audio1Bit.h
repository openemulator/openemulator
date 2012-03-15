
/**
 * libemulation
 * Audio 1-bit
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a 1-bit audio device
 */

#ifndef _AUDIO1BIT_H
#define _AUDIO1BIT_H

#include "OEComponent.h"

class Audio1Bit : public OEComponent
{
public:
    Audio1Bit();
    
    bool setValue(string name, string value);
    bool getValue(string name, string& value);
    bool setRef(string name, OEComponent *ref);
    bool init();
    void update();
    
protected:
    bool readAudioInput();
    void toggleAudioOutput();
    
private:
    OEComponent *audioCodec;
    
    float noiseRejection;
    float volume;
    
    float inputThreshold;
    OEInt16 inputLevel;
    
    bool outputState;
	OEInt16 outputLevel;
};

#endif

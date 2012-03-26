
/**
 * libemulation
 * OESound
 * (C) 2012 by Marc S. Ressl (mressl@umich.edu)
 * Released under the GPL
 *
 * Implements a sound type
 */

#ifndef _OESOUND_H
#define _OESOUND_H

#include "OECommon.h"

class OESound
{
public:
    OESound();
    OESound(string path);
    OESound(OEData& data);
    
    float getSampleRate();
    int getChannelNum();
    int getFrameNum();
    float *getSamples();
    
    bool load(string path);
    bool load(OEData& data);
    
private:
	float sampleRate;
	int channelNum;
    
    vector<float> samples;
    
    void init();
};

#endif
